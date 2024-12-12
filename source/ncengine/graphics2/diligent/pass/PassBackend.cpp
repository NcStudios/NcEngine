#include "PassBackend.h"
#include "graphics2/diligent/pass/PassUtilities.h"
#include "graphics2/diligent/resource/PerPassResourceSignature.h"
#include "graphics2/diligent/resource/PostProcessPropertyBufferResource.h"
#include "graphics2/diligent/resource/ResourceTypes.h"
#include "graphics2/diligent/resource/ShaderBindings.h"
#include "graphics2/diligent/resource/WireframeBufferResource.h"
#include "graphics2/frontend/subsystem/PostProcessState.h"
#include "ncengine/debug/Profile.h"

#include "ncutility/NcError.h"

#include <ranges>

namespace
{
using namespace nc;
using namespace nc::graphics;

auto UpdateBuffer(Diligent::IDeviceContext& context,
                  const PostProcessPassProperties& properties,
                  PostProcessPipelineInstance& instance)
{
    std::visit(
        [&context, &instance](const auto& unpacked){
            NC_ASSERT(instance.buffer.has_value(), "Pass instance does not have a UniformBuffer");
            instance.buffer->Write(context, unpacked);
        },
        properties
    );
}

void EnableInstance(PostProcessEffectId effectId,
                    PostProcessPass& pass)
{
    pass.anyEnabled = true;
    auto instance = std::ranges::find(pass.instances, effectId, &PostProcessPipelineInstance::effectId);
    NC_ASSERT(instance != pass.instances.end(), "Post process pass instance not found");
    instance->enabled = true;
}

void DisableInstance(PostProcessEffectId effectId,
                     PostProcessPass& pass)
{
    auto anyEnabled = false;
    for (auto& instance : pass.instances)
    {
        if (instance.effectId == effectId)
        {
            instance.enabled = false;
        }
        else
        {
            anyEnabled = anyEnabled || instance.enabled;
        }
    }

    pass.anyEnabled = anyEnabled;
}

auto FindInstance(std::vector<PostProcessPass>& passes,
                  PostProcessEffectId effectId,
                  PostProcessPassFlag::type passId) -> PostProcessPipelineInstance&
{
    auto pass = std::ranges::find_if(passes, [passId](auto& ppPass) { return ppPass.passDesc.id == passId; });
    if (pass != passes.end())
    {
        auto instance = std::ranges::find(pass->instances, effectId, &PostProcessPipelineInstance::effectId);
        if (instance != pass->instances.end())
        {
            return *instance;
        }
    }

    NC_ASSERT(false, fmt::format(
        "Post process effect/pass mismatch: '{}'/'{}'.",
        effectId,
        passId
    ));

    std::unreachable();
}

auto ToDrawAttribs(const nc::graphics::Batch& batch) -> Diligent::DrawIndexedAttribs
{
    constexpr auto drawFlags = Diligent::DRAW_FLAG_VERIFY_ALL |
                               Diligent::DRAW_FLAG_DYNAMIC_RESOURCE_BUFFERS_INTACT;

    return Diligent::DrawIndexedAttribs{
        batch.indexCount,
        Diligent::VT_UINT32,
        drawFlags,
        batch.instanceCount,
        batch.firstIndex,
        batch.vertexOffset,
        batch.firstInstance
    };
}

void DrawIndexed(Diligent::IDeviceContext& context, const std::vector<nc::graphics::Batch>& batches)
{
    NC_PROFILE_SCOPE("DrawIndexed()", nc::ProfileCategory::Rendering);
    for (const auto& batch : batches)
    {
        context.DrawIndexed(ToDrawAttribs(batch));
    }
}
} // anonymous namespace

namespace nc::graphics
{
PassBackend::PassBackend(Diligent::IRenderDevice& device,
                         Diligent::IDeviceContext& context,
                         Diligent::ISwapChain& swapChain,
                         ShaderFactory& shaderFactory,
                         ShaderBindings& shaderBindings,
                         const PassManifest& passManifest)
{
    m_materialPasses = MakeMaterialPasses
    (
        device,
        shaderFactory,
        shaderBindings,
        passManifest.MaterialPassDescs()
    );

    m_wireframePass = std::make_unique<WireframePass>
    (
        device,
        shaderFactory,
        shaderBindings,
        passManifest.WireframePassDesc()
    );

    m_postProcessPasses = MakePostProcessPasses
    (
        device,
        context,
        swapChain,
        shaderFactory,
        shaderBindings,
        passManifest
    );

    // Create a hardcoded dummy post process pass that always pipes the result of the last rendered render target to the swapchain
    auto shaderPaths = ShaderPaths{"PPEnd.psh", "PostProcess.vsh"};
    auto finalPass = PassDesc
    {
        .id = ToPassBaseId(shaderPaths),
        .name = "Dummy Pass",
        .type = PassType::PostProcess,
        .shaderPaths = shaderPaths,
        .colorSources = SingleSource(passManifest.FinalColorTarget()),
        .colorSink = SwapChainColorRTIndex
    };

    m_finalPass = std::make_unique<PostProcessPass>(MakePostProcessPass(device, context, swapChain, shaderFactory, shaderBindings, finalPass));

    // Make all the off screen render targets that will be used by the passes
    auto& postProcessColorSinks = shaderBindings.GetPerPassSignature().GetPostProcessColorSinkBufferResource();
    postProcessColorSinks.Add(device, passManifest.ColorSinkCount(), swapChain.GetDesc().Width, swapChain.GetDesc().Height);

    auto& postProcessDepthSinks = shaderBindings.GetPerPassSignature().GetPostProcessDepthSinkBufferResource();
    postProcessDepthSinks.Add(device, passManifest.DepthSinkCount(), swapChain.GetDesc().Width, swapChain.GetDesc().Height);
}

void PassBackend::Update(Diligent::IDeviceContext& context, const PostProcessState& postProcessState)
{
    for (const auto& [effectId, effectPasses, enabled] : postProcessState.toggledEffects)
    {
        for (auto& pass : m_postProcessPasses)
        {
            if (pass.passDesc.id & effectPasses)
            {
                enabled ? EnableInstance(effectId, pass) : DisableInstance(effectId, pass);
            }
        }
    }

    for (const auto& [effectId, passId, properties] : postProcessState.modifiedProperties)
    {
        UpdateBuffer(context, properties, FindInstance(m_postProcessPasses, effectId, passId));
    }
}

void PassBackend::RenderMaterial(Diligent::IDeviceContext& context,
                                 Diligent::ISwapChain& swapChain,
                                 PerPassResourceSignature& perPassResourceSignature,
                                 const std::vector<std::vector<Batch>>& passBatches)
{
    NC_PROFILE_SCOPE("PassBackend::RenderMaterial()", ProfileCategory::Rendering);
    NC_ASSERT(m_materialPasses.size() == passBatches.size(), "Frontend/Backend passes out of sync.");

    for (auto [pass, batches] : std::views::zip(m_materialPasses, passBatches))
    {
        BindRenderTarget(context, swapChain, perPassResourceSignature.GetPostProcessColorSinkBufferResource(), perPassResourceSignature.GetPostProcessDepthSinkBufferResource(), pass.colorRTIndex, pass.depthRTIndex);
        ClearRenderTarget(context, swapChain, perPassResourceSignature.GetPostProcessColorSinkBufferResource(), perPassResourceSignature.GetPostProcessDepthSinkBufferResource(), pass.colorRTIndex, pass.depthRTIndex);
        context.SetPipelineState(pass.pso);
        DrawIndexed(context, batches);
    }
}

void PassBackend::RenderPostProcess(Diligent::IDeviceContext& context,
                                    Diligent::ISwapChain& swapChain,
                                    PerPassResourceSignature& perPassResourceSignature)
{
    NC_PROFILE_SCOPE("PassBackend::RenderPostProcess()", ProfileCategory::Rendering);
    constexpr auto drawAttribs = Diligent::DrawAttribs{4, Diligent::DRAW_FLAG_VERIFY_ALL};

    auto enabledPassCount = 0u;
    for (auto& pass : m_postProcessPasses)
    {
        if (!pass.anyEnabled) continue;

        BindRenderTarget(context, swapChain, perPassResourceSignature.GetPostProcessColorSinkBufferResource(), perPassResourceSignature.GetPostProcessDepthSinkBufferResource(), pass.passDesc.colorSink, pass.passDesc.depthSink);
        ClearRenderTarget(context, swapChain, perPassResourceSignature.GetPostProcessColorSinkBufferResource(), perPassResourceSignature.GetPostProcessDepthSinkBufferResource(), pass.passDesc.colorSink, pass.passDesc.depthSink);
        context.SetPipelineState(pass.pso);

        perPassResourceSignature.GetPostProcessSinkIndexBufferResource().Update(context, pass.passDesc.colorSources, pass.passDesc.depthSources);

        for (auto& instance : pass.instances)
        {
            if (!instance.enabled)
            {
                continue;
            }

            if (instance.buffer.has_value())
            {
                //propertyBuffer.SetVariable(pass.passDesc.id, instance.buffer->GetBuffer());
            }

            context.Draw(drawAttribs);
        }
        context.TransitionShaderResources(&perPassResourceSignature.GetResourceBinding());
        enabledPassCount++;
    }

    // Render final post process pass
    auto finalPassColorSource = 0u;
    if (enabledPassCount)
    {
        finalPassColorSource = m_postProcessPasses.at(enabledPassCount-1).passDesc.colorSink;
    }

    BindRenderTarget(context, swapChain, perPassResourceSignature.GetPostProcessColorSinkBufferResource(), perPassResourceSignature.GetPostProcessDepthSinkBufferResource(), SwapChainColorRTIndex, SwapChainDepthRTIndex);
    ClearRenderTarget(context, swapChain, perPassResourceSignature.GetPostProcessColorSinkBufferResource(), perPassResourceSignature.GetPostProcessDepthSinkBufferResource(), SwapChainColorRTIndex, SwapChainDepthRTIndex);

    auto finalPassDepthSource = std::vector<uint32_t>{};
    perPassResourceSignature.GetPostProcessSinkIndexBufferResource().Update(context, SingleSource(finalPassColorSource), finalPassDepthSource);
    context.TransitionShaderResources(&perPassResourceSignature.GetResourceBinding());
    context.SetPipelineState(m_finalPass->pso);
    context.Draw(drawAttribs);
}

void PassBackend::RenderWireframe(Diligent::IDeviceContext& context,
                                  Diligent::ISwapChain& swapChain,
                                  PerPassResourceSignature& perPassResourceSignature,
                                  const WireframeRendererRenderState& state)
{
    NC_PROFILE_SCOPE("PassBackend::RenderWireframe()", ProfileCategory::Rendering);
    if (state.wireframeData.empty() || !m_wireframePass)
    {
        return;
    }

    BindRenderTarget(context, swapChain, perPassResourceSignature.GetPostProcessColorSinkBufferResource(), perPassResourceSignature.GetPostProcessDepthSinkBufferResource(), m_wireframePass->colorRTIndex, m_wireframePass->depthRTIndex);
    context.SetPipelineState(m_wireframePass->pso);
    for (const auto& [data, mesh] : state.wireframeData)
    {
        m_wireframePass->buffer->Update(context, data);
        const auto attribs = Diligent::DrawIndexedAttribs{
            mesh.indexCount,
            Diligent::VT_UINT32,
            Diligent::DRAW_FLAG_VERIFY_ALL,
            1,
            mesh.firstIndex,
            mesh.firstVertex,
            0
        };

        context.DrawIndexed(attribs);
    }
}
} // namespace nc::graphics
