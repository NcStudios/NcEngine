#include "PassBackend.h"
#include "PassUtilities.h"
#include "graphics2/diligent/resource/PerPassResourceSignature.h"
#include "graphics2/diligent/resource/PostProcessPropertyBufferResource.h"
#include "graphics2/diligent/resource/ResourceTypes.h"
#include "graphics2/diligent/resource/ShaderBindings.h"
#include "graphics2/diligent/resource/SinkIndexBufferResource.h"
#include "graphics2/diligent/resource/WireframeBufferResource.h"
#include "graphics2/frontend/subsystem/PostProcessState.h"
#include "ncengine/debug/Profile.h"

#include "ncutility/NcError.h"

#include <ranges>

namespace
{
using namespace nc;
using namespace nc::graphics;
using namespace Diligent;

void EnableInstance(PostProcessEffectId effectId, PostProcessPass& pass)
{
    pass.anyEnabled = true;
    auto instance = std::ranges::find(pass.instances, effectId, &PostProcessPipelineInstance::effectId);
    NC_ASSERT(instance != pass.instances.end(), "Post process pass instance not found");
    instance->enabled = true;
}

void DisableInstance(PostProcessEffectId effectId, PostProcessPass& pass)
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
    auto pass = std::ranges::find_if(passes, [passId](auto& ppPass) { return ppPass.id == passId; });
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

auto ToDrawAttribs(const Batch& batch) -> DrawIndexedAttribs
{
    constexpr auto drawFlags = DRAW_FLAG_VERIFY_ALL |
                               DRAW_FLAG_DYNAMIC_RESOURCE_BUFFERS_INTACT;

    return DrawIndexedAttribs{
        batch.indexCount,
        VT_UINT32,
        drawFlags,
        batch.instanceCount,
        batch.firstIndex,
        batch.vertexOffset,
        batch.firstInstance
    };
}

void DrawIndexed(IDeviceContext& context, const std::vector<Batch>& batches)
{
    NC_PROFILE_SCOPE("DrawIndexed()", nc::ProfileCategory::Rendering);
    for (const auto& batch : batches)
    {
        context.DrawIndexed(ToDrawAttribs(batch));
    }
}

void ResolveMsaaTextures(IDeviceContext& context, PerPassResourceSignature& perPassResourceSignature, uint32_t numSamples)
{
    if (numSamples <= 1)
    {
        return;
    }
    auto& colorSinkBuffer = perPassResourceSignature.GetColorSinksResource();
    
    for (auto i = 0u; i < colorSinkBuffer.GetMsaaSinkCount(); i++)
    {
        ResolveTextureSubresourceAttribs resolveAttribs;
        resolveAttribs.SrcTextureTransitionMode = RESOURCE_STATE_TRANSITION_MODE_TRANSITION;
        resolveAttribs.DstTextureTransitionMode = RESOURCE_STATE_TRANSITION_MODE_TRANSITION;
        context.ResolveTextureSubresource(colorSinkBuffer.GetMsaaTexture(i), colorSinkBuffer.GetTexture(i), resolveAttribs);
    }
}
} // anonymous namespace

namespace nc::graphics
{
using namespace Diligent;

PassBackend::PassBackend(IRenderDevice& device,
                         IDeviceContext& context,
                         ISwapChain& swapChain,
                         ShaderFactory& shaderFactory,
                         ShaderBindings& shaderBindings,
                         const PassManifest& passManifest,
                         uint32_t numSamples)
    : m_numSamples{numSamples},
      m_finalColorTarget{std::nullopt},
      m_finalPostProcessTarget{std::nullopt}
{
    // Get sink buffers
    auto& perPassSignature = shaderBindings.GetPerPassSignature();
    auto& colorSinks = perPassSignature.GetColorSinksResource();
    auto& depthSinks = perPassSignature.GetDepthSinksResource();

    // Get swapchain width and height to create screen-sized render targets
    auto screenWidth = swapChain.GetDesc().Width;
    auto screenHeight = swapChain.GetDesc().Height;

    // Make all the off screen render targets that will be used by the passes
    colorSinks.Add(device, context, passManifest.ColorSinkCount(), screenWidth, screenHeight);
    depthSinks.Add(device, context, passManifest.DepthSinkCount(), screenWidth, screenHeight);

    // Make all of the offscreen MSAA render targets that will be used by the passes. 
    if (m_numSamples > 1)
    {
        colorSinks.Add(device, context, passManifest.ColorSinkCount(), screenWidth, screenHeight, m_numSamples);
        depthSinks.Add(device, context, passManifest.DepthSinkCount(), screenWidth, screenHeight, m_numSamples);
    }

    // Make all the post process render targets that will be used by the passes
    NC_ASSERT(passManifest.PostProcessSinkCount() == perPassSignature.GetPostProcessSinkCount(), 
    "Mismatch between the number of post process sinks in the manifest and post process resource slots.");
    for (auto i = 0u; i < passManifest.PostProcessSinkCount(); i++)
    {
        auto& postProcessSink = perPassSignature.GetPostProcessResource(i);
        postProcessSink.Add(device, context, 1, screenWidth, screenHeight);
    }

    // Make the pass and pipeline objects
    MakePassesAndPipelines(device, swapChain, shaderFactory, shaderBindings, passManifest);
}

void PassBackend::Update(const PostProcessState& postProcessState)
{
    for (const auto& [effectId, effectPasses, enabled] : postProcessState.toggledEffects)
    {
        for (auto& pass : m_postProcessPasses)
        {
            if (pass.id & effectPasses)
            {
                enabled ? EnableInstance(effectId, pass) : DisableInstance(effectId, pass);
            }
        }
    }

    for (const auto& [effectId, passId, properties] : postProcessState.modifiedProperties)
    {
        FindInstance(m_postProcessPasses, effectId, passId).properties = properties;
    }

    m_finalColorTarget = std::nullopt;
    m_finalPostProcessTarget = std::nullopt;
}

void PassBackend::RenderMaterial(IDeviceContext& context,
                                 ISwapChain& swapChain,
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
        m_finalColorTarget = staticPass.sinks.color;

        // PassManifest verifies static/skinned pass pairs specify the same render targets, so we can just choose from either here.
        BindRenderTarget(context, swapChain, perPassResourceSignature, staticPass.sinks.color, staticPass.sinks.depth, staticPass.isMsaa && m_numSamples > 1);
        ClearRenderTarget(context, swapChain, perPassResourceSignature, staticPass.sinks.color, staticPass.sinks.depth, staticPass.isMsaa && m_numSamples > 1);

        context.SetPipelineState(staticPass.pso);
        DrawIndexed(context, staticBatches);
        context.SetPipelineState(skinnedPass.pso);
        DrawIndexed(context, skinnedBatches);
    }
}

void PassBackend::RenderWireframe(IDeviceContext& context,
                                  ISwapChain& swapChain,
                                  PerPassResourceSignature& perPassResourceSignature,
                                  const WireframeRendererRenderState& state)
{
    NC_PROFILE_SCOPE("PassBackend::RenderWireframe()", ProfileCategory::Rendering);
    if (state.wireframeData.empty() || !m_wireframePass)
    {
        return;
    }
    
    m_finalColorTarget = m_wireframePass->sinks.color;
    BindRenderTarget(context, swapChain, perPassResourceSignature, m_wireframePass->sinks.color, m_wireframePass->sinks.depth, m_wireframePass->isMsaa && m_numSamples > 1);
    context.SetPipelineState(m_wireframePass->pso);

    for (const auto& [data, mesh] : state.wireframeData)
    {
        m_wireframePass->buffer->Update(context, data);
        const auto attribs = DrawIndexedAttribs{
            mesh.indexCount,
            VT_UINT32,
            DRAW_FLAG_VERIFY_ALL,
            1,
            mesh.firstIndex,
            mesh.firstVertex,
            0
        };

        context.DrawIndexed(attribs);
    }
}

void PassBackend::RenderParticle(IDeviceContext& context,
                                 ISwapChain& swapChain,
                                 PerPassResourceSignature& perPassResourceSignature,
                                 const ParticleRenderState& state)
{
    if (state.particleData.instances.empty() || !m_particlePass)
    {
        return;
    }

    m_finalColorTarget = m_particlePass->sinks.color;
    BindRenderTarget(context, swapChain, perPassResourceSignature, m_particlePass->sinks.color, m_particlePass->sinks.depth, m_particlePass->isMsaa && m_numSamples > 1);

    context.SetPipelineState(m_particlePass->pso);
    const auto attribs = DrawIndexedAttribs{
        state.mesh.indexCount,
        VT_UINT32,
        DRAW_FLAG_VERIFY_ALL,
        static_cast<uint32_t>(state.particleData.instances.size()),
        state.mesh.firstIndex,
        state.mesh.firstVertex,
        0
    };

    context.DrawIndexed(attribs);
}

void PassBackend::RenderPostProcess(IDeviceContext& context,
                                    ISwapChain& swapChain,
                                    PerPassResourceSignature& perPassResourceSignature,
                                    PerFrameResourceSignature& perFrameResourceSignature)
{
    NC_PROFILE_SCOPE("PassBackend::RenderPostProcess()", ProfileCategory::Rendering);
    constexpr auto drawAttribs = DrawAttribs{4, DRAW_FLAG_VERIFY_ALL};
    auto& propertyBuffer = perFrameResourceSignature.GetPostProcessPropertyBuffer();
    auto& sinkIndexBuffer = perPassResourceSignature.GetSinkIndexBufferResource();

    // If MSAA samples are set to be greater than 1 in the config, all PassType::Material, PassType::SkinnedMaterial and PassType::Misc passes are multisampled.
    // These need to be resolved before used by the post process passes.
    ResolveMsaaTextures(context, perPassResourceSignature, m_numSamples);
    context.TransitionShaderResources(&perPassResourceSignature.GetResourceBinding());

    for (const auto& pass : m_postProcessPasses)
    {
        if (!pass.anyEnabled) continue;

        m_finalPostProcessTarget = pass.sinks.postProcess;

        // Get the post process resource we are writing to to bind in the next step
        auto& postProcessSinkBuffer = perPassResourceSignature.GetPostProcessResource(pass.sinks.postProcess);

        BindRenderTarget(context, swapChain, postProcessSinkBuffer, pass.sinks.postProcess);
        ClearRenderTarget(context, swapChain, postProcessSinkBuffer, pass.sinks.postProcess);

        // If this post process pass consumes any post process pass as a source, bind that source's shader resource view to the SRB.
        auto hasPostProcessSource = pass.sources.postProcess != NoTarget;
        if (hasPostProcessSource)
        {
            auto& postProcessSourceBuffer = perPassResourceSignature.GetPostProcessResource(pass.sources.postProcess);
            postProcessSourceBuffer.Update();
            perPassResourceSignature.Commit(context);
        }
        sinkIndexBuffer.Update(context, pass.sources.color, pass.sources.depth, hasPostProcessSource);
        context.SetPipelineState(pass.pso);

        for (auto& instance : pass.instances)
        {
            if (!instance.enabled) continue;
            if (instance.properties.has_value())
            {
                propertyBuffer.Update(context, instance.properties.value());
            }
            context.Draw(drawAttribs);
        }
    }
}

void PassBackend::RenderOutputToSwapchain(IDeviceContext& context, ISwapChain& swapChain, PerPassResourceSignature& perPassResourceSignature)
{
    NC_PROFILE_SCOPE("PassBackend::RenderOutputToSwapchain()", ProfileCategory::Rendering);
    constexpr auto drawAttribs = DrawAttribs{4, DRAW_FLAG_VERIFY_ALL};
    auto& sinkIndexBuffer = perPassResourceSignature.GetSinkIndexBufferResource();

    // Render final post process pass
    // Bind the swapchain as the render target
    BindRenderTarget(context, swapChain, perPassResourceSignature, m_finalPass->sinks.color, m_finalPass->sinks.depth, false);
    ClearRenderTarget(context, swapChain, perPassResourceSignature, m_finalPass->sinks.color, m_finalPass->sinks.depth, false);

    // This final pass renders the last pass in the chain's sink target to the swapchain. It's either a color target or a post process target,
    // depending on whether the post process passes are enabled or disabled.
    m_finalPass->sources.color.clear();
    auto hasPostProcess = false;
    if (m_finalPostProcessTarget.has_value())  // The last pass in the chain is a post process target
    {
        m_finalPass->sources.postProcess = m_finalPostProcessTarget.value();
        auto& postProcessSourceBuffer = perPassResourceSignature.GetPostProcessResource(m_finalPass->sources.postProcess);
        postProcessSourceBuffer.Update();
        perPassResourceSignature.Commit(context);
        hasPostProcess = true;
    }
    else // The last pass in the chain is a color target
    {
        m_finalPass->sources.color.push_back(m_finalColorTarget.value());
        hasPostProcess = false;
    }

    sinkIndexBuffer.Update(context, m_finalPass->sources.color, std::vector<uint32_t>(), hasPostProcess);
    context.SetPipelineState(m_finalPass->pso);
    context.Draw(drawAttribs);
}

void PassBackend::MakePassesAndPipelines(IRenderDevice& device,
                             ISwapChain& swapChain,
                             ShaderFactory& shaderFactory,
                             ShaderBindings& shaderBindings,
                             const PassManifest& passManifest)
{
    // Create the static material passes
    m_staticMaterialPasses.reserve(passManifest.StaticMaterialPassDescs().size());
    for (const auto& passDesc : passManifest.StaticMaterialPassDescs())
    {
        m_staticMaterialPasses.emplace_back(device, shaderFactory, shaderBindings, passManifest, passDesc, m_numSamples);
    }

    // Create the skinned material passes
    m_skinnedMaterialPasses.reserve(passManifest.SkinnedMaterialPassDescs().size());
    for (auto& passDesc : passManifest.SkinnedMaterialPassDescs())
    {
        m_skinnedMaterialPasses.emplace_back(device, shaderFactory, shaderBindings, passManifest, passDesc, m_numSamples);
    }

    // Create the wireframe pass
    m_wireframePass = std::make_unique<WireframePass>(device, shaderFactory, shaderBindings, passManifest, passManifest.WireframePassDesc(), m_numSamples);

    // Create the particle pass
    m_particlePass = std::make_unique<ParticlePass>(device, shaderFactory, shaderBindings, passManifest, passManifest.ParticlePassDesc(), m_numSamples);
    m_finalColorTarget = m_particlePass->sinks.color;

    // Create the post process passes
    m_postProcessPasses.reserve(passManifest.PostProcessPassDescs().size());
    for (const auto& passDesc : passManifest.PostProcessPassDescs())
    {
        m_postProcessPasses.emplace_back(device, swapChain, shaderFactory, shaderBindings, passManifest, passDesc);
    }
    m_finalPostProcessTarget = m_postProcessPasses.back().sinks.postProcess;

    // Create a hardcoded dummy post process pass that always pipes the result of the last rendered render target to the swapchain
    auto shaderPaths = ShaderPaths{"PPEnd.psh", "PostProcess.vsh"};
    auto name = "Final Pass";
    m_finalPass = std::make_unique<PostProcessPass>(device, swapChain, shaderFactory, shaderBindings, passManifest, PassDesc
    {
        .id = ToPassBaseId(shaderPaths, name),
        .name = name,
        .type = PassType::PostProcess,
        .shaderPaths = shaderPaths,
        .colorSink = ColorTarget::Swapchain,
        .depthSink = DepthTarget::DepthStencil,
        .useDepthTest = false
    });
    m_finalPass->sources.postProcess = m_finalPostProcessTarget.value();
}
} // namespace nc::graphics
