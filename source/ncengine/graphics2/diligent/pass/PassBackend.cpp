#include "PassBackend.h"
#include "PassUtilities.h"
#include "graphics2/diligent/resource/PerPassResourceSignature.h"
#include "graphics2/diligent/resource/PostProcessPropertyBufferResource.h"
#include "graphics2/diligent/resource/PostProcessSinkIndexBufferResource.h"
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
                         Diligent::ISwapChain& swapChain,
                         ShaderFactory& shaderFactory,
                         ShaderBindings& shaderBindings,
                         const PassManifest& passManifest)
{
    m_staticMaterialPasses = MakeMaterialPasses
    (
        device,
        shaderFactory,
        shaderBindings,
        passManifest.StaticMaterialPassDescs()
    );

    m_skinnedMaterialPasses = MakeMaterialPasses
    (
        device,
        shaderFactory,
        shaderBindings,
        passManifest.SkinnedMaterialPassDescs()
    );

    m_skinnedMaterialPasses = MakeMaterialPasses
    (
        device,
        shaderFactory,
        shaderBindings,
        passManifest.SkinnedMaterialPassDescs()
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
        swapChain,
        shaderFactory,
        shaderBindings,
        passManifest
    );

    // Create a hardcoded dummy post process pass that always pipes the result of the last rendered render target to the swapchain
    auto shaderPaths = ShaderPaths{"PPEnd.psh", "PostProcess.vsh"};
    auto name = "Final Pass";
    auto finalPass = PassDesc
    {
        .id = ToPassBaseId(shaderPaths, name),
        .name = name,
        .type = PassType::PostProcess,
        .shaderPaths = shaderPaths,
        .colorSources = SingleSource(FinalColorTarget()),
        .colorSink = SwapChainColorRTIndex,
        .depthSink = SwapChainDepthRTIndex
    };

    m_finalPass = std::make_unique<PostProcessPass>(MakePostProcessPass(device, swapChain, shaderFactory, shaderBindings, finalPass));

    // Make all the off screen render targets that will be used by the passes
    auto& postProcessColorSinks = shaderBindings.GetPerPassSignature().GetPostProcessColorSinkBufferResource();
    postProcessColorSinks.Add(device, passManifest.ColorSinkCount(), swapChain.GetDesc().Width, swapChain.GetDesc().Height);

    auto& postProcessDepthSinks = shaderBindings.GetPerPassSignature().GetPostProcessDepthSinkBufferResource();
    postProcessDepthSinks.Add(device, passManifest.DepthSinkCount(), swapChain.GetDesc().Width, swapChain.GetDesc().Height);
}

void PassBackend::Update(const PostProcessState& postProcessState)
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
        FindInstance(m_postProcessPasses, effectId, passId).properties = properties;
    }
}

void PassBackend::RenderMaterial(Diligent::IDeviceContext& context,
                                 Diligent::ISwapChain& swapChain,
                                 PerPassResourceSignature& perPassResourceSignature,
                                 const std::vector<std::vector<Batch>>& staticPassBatches,
                                 const std::vector<std::vector<Batch>>& skinnedPassBatches)
{
    NC_PROFILE_SCOPE("PassBackend::RenderMaterial()", ProfileCategory::Rendering);
    NC_ASSERT(
        m_staticMaterialPasses.size() == staticPassBatches.size() &&
        m_skinnedMaterialPasses.size() == skinnedPassBatches.size(),
        "Frontend/Backend passes out of sync."
    );

    auto passView = std::views::zip(
        m_staticMaterialPasses,
        m_skinnedMaterialPasses,
        staticPassBatches,
        skinnedPassBatches
    );

    for (auto [staticPass, skinnedPass, staticBatches, skinnedBatches] : passView)
    {
        // PassManifest verifies static/skinned pass pairs specify the same render targets, so we can just choose from either here.
        BindRenderTarget(context, swapChain,
                         perPassResourceSignature.GetPostProcessColorSinkBufferResource(),
                         perPassResourceSignature.GetPostProcessDepthSinkBufferResource(),
                         staticPass.colorRTIndex, staticPass.depthRTIndex);
        ClearRenderTarget(context, swapChain,
                          perPassResourceSignature.GetPostProcessColorSinkBufferResource(),
                          perPassResourceSignature.GetPostProcessDepthSinkBufferResource(),
                          staticPass.colorRTIndex, staticPass.depthRTIndex);

        context.SetPipelineState(staticPass.pso);
        DrawIndexed(context, staticBatches);
        context.SetPipelineState(skinnedPass.pso);
        DrawIndexed(context, skinnedBatches);
    }
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

    BindRenderTarget(context, swapChain,
                     perPassResourceSignature.GetPostProcessColorSinkBufferResource(),
                     perPassResourceSignature.GetPostProcessDepthSinkBufferResource(),
                     m_wireframePass->colorRTIndex, m_wireframePass->depthRTIndex);
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

void PassBackend::RenderPostProcess(Diligent::IDeviceContext& context,
                                    Diligent::ISwapChain& swapChain,
                                    PerPassResourceSignature& perPassResourceSignature,
                                    PerFrameResourceSignature& perFrameResourceSignature)
{
    NC_PROFILE_SCOPE("PassBackend::RenderPostProcess()", ProfileCategory::Rendering);
    constexpr auto drawAttribs = Diligent::DrawAttribs{4, Diligent::DRAW_FLAG_VERIFY_ALL};
    auto& propertyBuffer = perFrameResourceSignature.GetPostProcessPropertyBuffer();

    for (auto& pass : m_postProcessPasses)
    {
        if (!pass.anyEnabled) continue;

        BindRenderTarget(context, swapChain,
                         perPassResourceSignature.GetPostProcessColorSinkBufferResource(),
                         perPassResourceSignature.GetPostProcessDepthSinkBufferResource(),
                         pass.passDesc.colorSink, pass.passDesc.depthSink);
        ClearRenderTarget(context, swapChain,
                          perPassResourceSignature.GetPostProcessColorSinkBufferResource(),
                          perPassResourceSignature.GetPostProcessDepthSinkBufferResource(),
                          pass.passDesc.colorSink, pass.passDesc.depthSink);

        context.SetPipelineState(pass.pso);
        perPassResourceSignature.GetPostProcessSinkIndexBufferResource().Update(context, pass.passDesc.colorSources, pass.passDesc.depthSources);

        for (auto& instance : pass.instances)
        {
            if (!instance.enabled) continue;
            if (instance.properties.has_value())
            {
                propertyBuffer.Update(context, instance.properties.value());
            }
            context.Draw(drawAttribs);
        }
        context.TransitionShaderResources(&perPassResourceSignature.GetResourceBinding());
    }

    // Render final post process pass
    BindRenderTarget(context, swapChain,
                     perPassResourceSignature.GetPostProcessColorSinkBufferResource(),
                     perPassResourceSignature.GetPostProcessDepthSinkBufferResource(),
                     m_finalPass->passDesc.colorSink,  m_finalPass->passDesc.depthSink);
    ClearRenderTarget(context, swapChain,
                      perPassResourceSignature.GetPostProcessColorSinkBufferResource(),
                      perPassResourceSignature.GetPostProcessDepthSinkBufferResource(),
                      m_finalPass->passDesc.colorSink, m_finalPass->passDesc.depthSink);

    context.SetPipelineState(m_finalPass->pso);
    perPassResourceSignature.GetPostProcessSinkIndexBufferResource().Update(context, SingleSource(FinalColorTarget()), NoTargets());
    context.Draw(drawAttribs);
    context.TransitionShaderResources(&perPassResourceSignature.GetResourceBinding());
}

auto PassBackend::FinalColorTarget() const -> uint32_t
{
    for (auto& postProcessPass : std::ranges::reverse_view(m_postProcessPasses))
    {
        if (postProcessPass.anyEnabled)
        {
            return postProcessPass.passDesc.colorSink;
        }
    }

    if (m_wireframePass)
    {
        return m_wireframePass->colorRTIndex;
    }

    for (auto& materialPass : std::ranges::reverse_view(m_staticMaterialPasses))
    {
        const auto implementedPasses = GetImplementedMaterialPassFlags();
        auto pos = std::ranges::find_if(implementedPasses, [materialPass](auto& passFlag){ return materialPass.id == passFlag; });
        if (pos != implementedPasses.end())
        {
            return materialPass.colorRTIndex;
        }
    }

    return 0u;
}
} // namespace nc::graphics
