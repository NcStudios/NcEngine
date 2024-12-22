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
                         const PassManifest& passManifest,
                         uint32_t numSamples)
    : m_numSamples{numSamples},
      m_colorSinkCountMsaa{0u},
      m_depthSinkCountMsaa{0u}
{
    // Sink target buffers
    auto& postProcessColorSinks = shaderBindings.GetPerPassSignature().GetPostProcessColorSinkBufferResource();
    auto& postProcessDepthSinks = shaderBindings.GetPerPassSignature().GetPostProcessDepthSinkBufferResource();

    m_staticMaterialPasses = MakeMaterialPasses
    (
        device,
        shaderFactory,
        shaderBindings,
        passManifest.StaticMaterialPassDescs(),
        m_numSamples
    );

    m_skinnedMaterialPasses = MakeMaterialPasses
    (
        device,
        shaderFactory,
        shaderBindings,
        passManifest.SkinnedMaterialPassDescs(),
        m_numSamples
    );

    m_wireframePass = std::make_unique<WireframePass>
    (
        device,
        shaderFactory,
        shaderBindings,
        passManifest.WireframePassDesc(),
        m_numSamples
    );

    m_particlePass = std::make_unique<ParticlePass>
    (
        device,
        shaderFactory,
        shaderBindings,
        passManifest.ParticlePassDesc()
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
    postProcessColorSinks.Add(device, passManifest.ColorSinkCount(), swapChain.GetDesc().Width, swapChain.GetDesc().Height);
    postProcessDepthSinks.Add(device, passManifest.DepthSinkCount(), swapChain.GetDesc().Width, swapChain.GetDesc().Height);

    if (m_numSamples > 1)
    {
        m_colorSinkCountMsaa = passManifest.ColorSinkCountMsaa();
        m_depthSinkCountMsaa = passManifest.DepthSinkCountMsaa();
        postProcessColorSinks.Add(device, m_colorSinkCountMsaa, swapChain.GetDesc().Width, swapChain.GetDesc().Height, m_numSamples);
        postProcessDepthSinks.Add(device, m_depthSinkCountMsaa, swapChain.GetDesc().Width, swapChain.GetDesc().Height, m_numSamples);
    }
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

    auto& colorSinkBuffer = perPassResourceSignature.GetPostProcessColorSinkBufferResource();
    auto& depthSinkBuffer = perPassResourceSignature.GetPostProcessDepthSinkBufferResource();

    for (auto [staticPass, skinnedPass, staticBatches, skinnedBatches] : passView)
    {
        // PassManifest verifies static/skinned pass pairs specify the same render targets, so we can just choose from either here.
        BindRenderTarget(context, swapChain, colorSinkBuffer, depthSinkBuffer, staticPass.colorRTIndex, staticPass.depthRTIndex, m_numSamples > 1);
        ClearRenderTarget(context, swapChain, colorSinkBuffer, depthSinkBuffer, staticPass.colorRTIndex, staticPass.depthRTIndex, m_numSamples > 1);

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

    auto& colorSinkBuffer = perPassResourceSignature.GetPostProcessColorSinkBufferResource();
    auto& depthSinkBuffer = perPassResourceSignature.GetPostProcessDepthSinkBufferResource();

    BindRenderTarget(context, swapChain, colorSinkBuffer, depthSinkBuffer, m_wireframePass->colorRTIndex, m_wireframePass->depthRTIndex, m_numSamples > 1);
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

void PassBackend::RenderParticle(Diligent::IDeviceContext& context,
                                 Diligent::ISwapChain& swapChain,
                                 PerPassResourceSignature& perPassResourceSignature,
                                 const ParticleRenderState& state)
{
    if (state.particleData.instances.empty() || !m_particlePass)
    {
        return;
    }

    BindRenderTarget(context, swapChain,
                     perPassResourceSignature.GetPostProcessColorSinkBufferResource(),
                     perPassResourceSignature.GetPostProcessDepthSinkBufferResource(),
                     m_particlePass->colorRTIndex,
                     m_particlePass->depthRTIndex,
                     m_numSamples > 1);

    context.SetPipelineState(m_particlePass->pso);
    const auto attribs = Diligent::DrawIndexedAttribs{
        state.mesh.indexCount,
        Diligent::VT_UINT32,
        Diligent::DRAW_FLAG_VERIFY_ALL,
        static_cast<uint32_t>(state.particleData.dirtyRanges.at(0).count),
        state.mesh.firstIndex,
        state.mesh.firstVertex,
        0
    };

    context.DrawIndexed(attribs);
}

void PassBackend::RenderPostProcess(Diligent::IDeviceContext& context,
                                    Diligent::ISwapChain& swapChain,
                                    PerPassResourceSignature& perPassResourceSignature,
                                    PerFrameResourceSignature& perFrameResourceSignature)
{
    NC_PROFILE_SCOPE("PassBackend::RenderPostProcess()", ProfileCategory::Rendering);
    constexpr auto drawAttribs = Diligent::DrawAttribs{4, Diligent::DRAW_FLAG_VERIFY_ALL};
    auto& propertyBuffer = perFrameResourceSignature.GetPostProcessPropertyBuffer();
    auto& colorSinkBuffer = perPassResourceSignature.GetPostProcessColorSinkBufferResource();
    auto& depthSinkBuffer = perPassResourceSignature.GetPostProcessDepthSinkBufferResource();
    auto& sinkIndexBuffer = perPassResourceSignature.GetPostProcessSinkIndexBufferResource();

    // If MSAA samples are set to be greater than 1 in the config, all PassType::Material, PassType::SkinnedMaterial and PassType::Misc passes are multisampled.
    // These need to be resolved before used by the post process passes.
    if (m_numSamples > 1)
    {
        for (auto i = 0u; i < m_colorSinkCountMsaa; i++)
        {
            Diligent::ResolveTextureSubresourceAttribs resolveAttribs;
            resolveAttribs.SrcTextureTransitionMode = Diligent::RESOURCE_STATE_TRANSITION_MODE_TRANSITION;
            resolveAttribs.DstTextureTransitionMode = Diligent::RESOURCE_STATE_TRANSITION_MODE_TRANSITION;
            context.ResolveTextureSubresource(colorSinkBuffer.GetMsaaTexture(i), colorSinkBuffer.GetTexture(i), resolveAttribs);
        }
    }

    for (auto& pass : m_postProcessPasses)
    {
        if (!pass.anyEnabled) continue;

        BindRenderTarget(context, swapChain, colorSinkBuffer, depthSinkBuffer, pass.passDesc.colorSink, pass.passDesc.depthSink, false);
        ClearRenderTarget(context, swapChain, colorSinkBuffer, depthSinkBuffer, pass.passDesc.colorSink, pass.passDesc.depthSink, false);

        context.SetPipelineState(pass.pso);
        sinkIndexBuffer.Update(context, pass.passDesc.colorSources, pass.passDesc.depthSources);

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
    BindRenderTarget(context, swapChain, colorSinkBuffer, depthSinkBuffer, m_finalPass->passDesc.colorSink, m_finalPass->passDesc.depthSink, false);
    ClearRenderTarget(context, swapChain, colorSinkBuffer, depthSinkBuffer, m_finalPass->passDesc.colorSink, m_finalPass->passDesc.depthSink, false);

    context.SetPipelineState(m_finalPass->pso);
    sinkIndexBuffer.Update(context, SingleSource(FinalColorTarget()), NoTargets());
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
