#include "PassBackend.h"
#include "asset/AssetService.h"
#include "PassUtilities.h"
#include "graphics2/ShaderTypes.h"
#include "graphics2/diligent/resource/PerPassResourceSignature.h"
#include "graphics2/diligent/resource/PostProcessPropertyBufferResource.h"
#include "graphics2/diligent/resource/ResourceTypes.h"
#include "graphics2/diligent/resource/ShaderBindings.h"
#include "graphics2/diligent/resource/SinkIndexBufferResource.h"
#include "graphics2/diligent/resource/WireframeBufferResource.h"
#include "graphics2/frontend/subsystem/EnvironmentRenderState.h"
#include "graphics2/frontend/subsystem/PostProcessState.h"
#include "ncengine/config/Config.h"
#include "ncengine/debug/Profile.h"
#include "ncutility/ScopeExit.h"

#include "ncasset/DefaultAssets.h"
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
                  PostProcessPassFlag::type passFlag) -> PostProcessPipelineInstance&
{
    auto pass = std::ranges::find_if(passes, [passFlag](auto& ppPass) { return ppPass.flag == passFlag; });
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
        passFlag
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

void SetViewportAndScissor(Diligent::IDeviceContext& context,
                           const Diligent::SwapChainDesc& swapChainDesc,
                           const nc::Viewport& viewport)
{
    const auto pipelineViewport = Diligent::Viewport{viewport.TopLeft.x, viewport.TopLeft.y, viewport.Size.x, viewport.Size.y, 0.0f, 1.0f};
    context.SetViewports(1, &pipelineViewport, swapChainDesc.Width, swapChainDesc.Height);
    const auto pipelineScissor = Diligent::Rect{0, 0, static_cast<int32_t>(viewport.Size.x), static_cast<int32_t>(viewport.Size.y)};
    context.SetScissorRects(1, &pipelineScissor, swapChainDesc.Width, swapChainDesc.Height);
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
                         const config::GraphicsSettings& graphicsSettings,
                         const config::MemorySettings& memorySettings,
                         uint32_t numSamples)
    : m_numSamples{numSamples},
      m_finalColorTarget{std::nullopt},
      m_finalPostProcessTarget{std::nullopt},
      m_perPassResourceSignature{&shaderBindings.GetPerPassSignature()},
      m_castsShadows{graphicsSettings.useShadows},
      m_shadowMapResX{graphicsSettings.shadowMapResolution},
      m_maxDirectionalLights{memorySettings.maxDirectionalLights},
      m_maxSpotLights{memorySettings.maxSpotLights},
      m_maxPointLights{memorySettings.maxPointLights}
{
    // Get sink buffers
    auto& colorSinks = m_perPassResourceSignature->GetColorSinksResource();
    auto& depthSinks = m_perPassResourceSignature->GetDepthSinksResource();

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
    NC_ASSERT(passManifest.PostProcessSinkCount() == m_perPassResourceSignature->GetPostProcessSinkCount(), 
    "Mismatch between the number of post process sinks in the manifest and post process resource slots.");
    for (auto i = 0u; i < passManifest.PostProcessSinkCount(); i++)
    {
        auto& postProcessSink = m_perPassResourceSignature->GetPostProcessSinkResource(i);
        postProcessSink.Add(device, context, 1, screenWidth, screenHeight);
    }

    // Make the pass and pipeline objects
    MakePassesAndPipelines(device, swapChain, shaderFactory, shaderBindings, passManifest);
}

void PassBackend::Update(IRenderDevice& device,
                         IDeviceContext& context,
                         const PostProcessState& postProcessState)
{
    for (const auto& [effectId, effectPasses, enabled] : postProcessState.toggledEffects)
    {
        for (auto& pass : m_postProcessPasses)
        {
            if (pass.flag & effectPasses)
            {
                enabled ? EnableInstance(effectId, pass) : DisableInstance(effectId, pass);
            }
        }
    }

    for (const auto& [effectId, passFlag, properties] : postProcessState.modifiedProperties)
    {
        FindInstance(m_postProcessPasses, effectId, passFlag).properties = properties;
    }

    m_finalColorTarget = std::nullopt;
    m_finalPostProcessTarget = std::nullopt;

    if (m_pointSinksToCreate > 0)
    {
        auto& pointShadowMapSinks = m_perPassResourceSignature->GetPointShadowMapSinksResource();
        pointShadowMapSinks.Add(device, context, m_pointSinksToCreate, m_shadowMapResX, m_shadowMapResX);
        m_pointSinksToCreate = 0;
    }

    if (m_uniSinksToCreate > 0)
    {
        auto& uniShadowMapSinks = m_perPassResourceSignature->GetUniShadowMapSinksResource();
        uniShadowMapSinks.Add(device, context, m_uniSinksToCreate, m_shadowMapResX, m_shadowMapResX);
        m_uniSinksToCreate = 0;
    }
}

void PassBackend::OnBeforeSceneLoad(const Scene&)
{
    auto& uniShadowMapSinks = m_perPassResourceSignature->GetUniShadowMapSinksResource();
    auto& pointShadowMapSinks = m_perPassResourceSignature->GetPointShadowMapSinksResource();
    uniShadowMapSinks.Clear();
    pointShadowMapSinks.Clear();
}

void PassBackend::RenderShadowPass(IDeviceContext& context,
                                   const MaterialPass& staticPass,
                                   const MaterialPass& skinnedPass,
                                   const std::vector<Batch>& staticBatches,
                                   const std::vector<Batch>& skinnedBatches,
                                   const std::span<const LightData>& lights)
{
    NC_PROFILE_SCOPE("PassBackend::RenderShadowPass", ProfileCategory::Rendering);

    if (!m_castsShadows)
    {
        return;
    }

    auto& sinkIndexBuffer = m_perPassResourceSignature->GetSinkIndexBufferResource();
    auto& uniShadowMapSinks = m_perPassResourceSignature->GetUniShadowMapSinksResource();
    auto& pointShadowMapSinks = m_perPassResourceSignature->GetPointShadowMapSinksResource();
    bool hasSomeShadowCaster = false;
    auto pointRenderTargetIndex = 0u;
    auto uniRenderTargetIndex = 0u;
    auto uniShadowCastersCount = 0u;
    auto pointShadowCastersCount = 0u;

    // Get a count of all lights that cast shadows, to know how many textures (sinks) we need to allocate
    for (const auto& light : lights)
    {
        if (light.type == LightType::Point && light.castsShadows)
        {
            pointShadowCastersCount++;
            continue;
        }

        if (light.castsShadows)
        {
            uniShadowCastersCount++;
        }
    }

    // Make all the shadow map render targets that will be used by the passes
    auto uniSinksCount = uniShadowMapSinks.GetSinkCount();
    if (uniShadowCastersCount > uniSinksCount)
    {
        m_uniSinksToCreate = std::min(uniShadowCastersCount - uniSinksCount, m_maxDirectionalLights + m_maxSpotLights);
    }
    else
    {
        m_uniSinksToCreate = 0u;
    }

    auto pointSinksCount = pointShadowMapSinks.GetSinkCount();
    if (pointShadowCastersCount > pointSinksCount)
    {
        m_pointSinksToCreate = std::min(pointShadowCastersCount - pointSinksCount, m_maxPointLights);
    }
    else
    {
        m_pointSinksToCreate = 0u;
    }

    // LightDataIndex corresponds to the index the shader will use to index the LightData buffer.
    // PointRenderTargetIndex and uniRenderTargetIndex correspond to the index of the shadow map in the SinkBuffers.
    for (const auto& [lightDataIndex, light] : std::views::enumerate(lights))
    {
        auto lightIndex = static_cast<uint32_t>(lightDataIndex);

        if (!light.castsShadows)
        {
            continue;
        }

        hasSomeShadowCaster = true;

        if (staticPass.flag & MaterialPassFlag::UniShadow && light.type != LightType::Point && m_uniSinksToCreate == 0)
        {
            sinkIndexBuffer.Update(context, std::vector<uint32_t>{}, std::vector<uint32_t>{}, false, lightIndex);
            m_perPassResourceSignature->Commit(context);

            BindUniShadowMapRenderTarget(context, uniShadowMapSinks, uniRenderTargetIndex);
            ClearUniShadowMapRenderTarget(context, uniShadowMapSinks, uniRenderTargetIndex);

            context.SetPipelineState(staticPass.pso);
            DrawIndexed(context, staticBatches);
            context.SetPipelineState(skinnedPass.pso);
            DrawIndexed(context, skinnedBatches);
            uniRenderTargetIndex++;
        }
        else if (staticPass.flag & MaterialPassFlag::PointShadow && light.type == LightType::Point && m_pointSinksToCreate == 0) // Point lights have six faces to render, not one
        {
            // Iterate through face indices for the point light
            for (auto faceIndex = 0u; faceIndex < 6u; faceIndex++)
            {
                sinkIndexBuffer.Update(context, std::vector<uint32_t>{}, std::vector<uint32_t>{}, false, lightIndex, faceIndex);
                m_perPassResourceSignature->Commit(context);

                BindPointShadowMapRenderTarget(context, pointShadowMapSinks, pointRenderTargetIndex, faceIndex);
                ClearPointShadowMapRenderTarget(context, pointShadowMapSinks, pointRenderTargetIndex, faceIndex);
        
                context.SetPipelineState(staticPass.pso);
                DrawIndexed(context, staticBatches);
                context.SetPipelineState(skinnedPass.pso);
                DrawIndexed(context, skinnedBatches);
            }
            pointRenderTargetIndex++;
        }
    }

    if (!hasSomeShadowCaster)
    {
        // No shadow-casting lights; ensure buffer is updated because it will be discarded at the end of the frame and read from in the materials pass - so it needs to be updated before that read.
        sinkIndexBuffer.Update(context, std::vector<uint32_t>{}, std::vector<uint32_t>{}, false, std::numeric_limits<uint32_t>::max());
        m_perPassResourceSignature->Commit(context);
    }

    context.TransitionShaderResources(&m_perPassResourceSignature->GetResourceBinding());
}

void PassBackend::RenderSkybox(Diligent::IDeviceContext& context,
                               Diligent::ISwapChain& swapChain,
                               const nc::graphics::EnvironmentRenderState& environmentRenderState,
                               const Viewport& viewport)
{
    NC_PROFILE_SCOPE("PassBackend::RenderSkybox()", ProfileCategory::Rendering);
    if (!environmentRenderState.useSkybox || !m_skyboxPass)
    {
        return;
    }

    // We need to transition resource state manually for the color and depth target here.
    auto* colorTargetTexture = m_perPassResourceSignature->GetColorSinksResource().GetTexture(m_skyboxPass->sinks.color);
    auto* depthTargetTexture = m_perPassResourceSignature->GetDepthSinksResource().GetTexture(m_skyboxPass->sinks.depth);

    auto barriers = std::array<Diligent::StateTransitionDesc, 2>
    {
        Diligent::StateTransitionDesc(
            colorTargetTexture,
            Diligent::RESOURCE_STATE_UNKNOWN,
            Diligent::RESOURCE_STATE_RENDER_TARGET,
            Diligent::STATE_TRANSITION_FLAG_UPDATE_STATE
        ),
        Diligent::StateTransitionDesc(
            depthTargetTexture,
            Diligent::RESOURCE_STATE_UNKNOWN,
            Diligent::RESOURCE_STATE_DEPTH_WRITE,
            Diligent::STATE_TRANSITION_FLAG_UPDATE_STATE
        )
    };
    context.TransitionResourceStates(static_cast<uint32_t>(barriers.size()), barriers.data());

    colorTargetTexture->SetState(Diligent::RESOURCE_STATE_UNKNOWN); // Disables automatic resource state management for just this texture.
    depthTargetTexture->SetState(Diligent::RESOURCE_STATE_UNKNOWN); // Disables automatic resource state management for just this texture.

    m_finalColorTarget = m_skyboxPass->sinks.color;
    BindRenderTarget(context, swapChain, *m_perPassResourceSignature, m_skyboxPass->sinks.color, m_skyboxPass->sinks.depth, false);
    SetViewportAndScissor(context, swapChain.GetDesc(), viewport);
    context.SetPipelineState(m_skyboxPass->pso);

    m_perPassResourceSignature->Commit(context);
    colorTargetTexture->SetState(Diligent::RESOURCE_STATE_RENDER_TARGET); // Enables automatic resource state management for just this texture.
    depthTargetTexture->SetState(Diligent::RESOURCE_STATE_DEPTH_WRITE); // Enables automatic resource state management for just this texture.

    const auto meshAccessor = asset::AssetService<asset::MeshView>::Get()->Acquire(nc::asset::SkyboxMesh);
    const auto attribs = DrawIndexedAttribs{
        meshAccessor.indexCount,
        VT_UINT32,
        DRAW_FLAG_VERIFY_ALL,
        1,
        meshAccessor.firstIndex,
        meshAccessor.firstVertex,
        0
    };

    context.DrawIndexed(attribs);
}

void PassBackend::RenderMaterial(IDeviceContext& context,
                                 ISwapChain& swapChain,
                                 const std::vector<std::vector<Batch>>& staticPassBatches,
                                 const std::vector<std::vector<Batch>>& skinnedPassBatches,
                                 const std::span<const LightData>& lights,
                                 const Viewport& viewport)
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
        m_finalColorTarget = m_finalColorTarget.has_value() ? std::min(staticPass.sinks.color, m_finalColorTarget.value()) : staticPass.sinks.color;

        if (staticPass.flag & MaterialPassFlag::UniShadow || staticPass.flag & MaterialPassFlag::PointShadow)
        {
            RenderShadowPass(context, staticPass, skinnedPass, staticBatches, skinnedBatches, lights);
            continue;
        }

        // PassManifest verifies static/skinned pass pairs specify the same render targets, so we can just choose from either here.
        BindRenderTarget(context, swapChain, *m_perPassResourceSignature, staticPass.sinks.color, staticPass.sinks.depth, staticPass.isMsaa && m_numSamples > 1);
        ClearRenderTarget(context, swapChain, *m_perPassResourceSignature, staticPass.sinks.color, staticPass.sinks.depth, staticPass.isMsaa && m_numSamples > 1);
        SetViewportAndScissor(context, swapChain.GetDesc(), viewport);

        context.SetPipelineState(staticPass.pso);
        DrawIndexed(context, staticBatches);
        context.SetPipelineState(skinnedPass.pso);
        DrawIndexed(context, skinnedBatches);
    }
}

void PassBackend::RenderWireframe(IDeviceContext& context,
                                  ISwapChain& swapChain,
                                  const WireframeRendererRenderState& state,
                                  const Viewport& viewport)
{
    NC_PROFILE_SCOPE("PassBackend::RenderWireframe()", ProfileCategory::Rendering);
    if (state.wireframeData.empty() || !m_wireframePass)
    {
        return;
    }

    m_finalColorTarget = m_wireframePass->sinks.color;
    BindRenderTarget(context, swapChain, *m_perPassResourceSignature, m_wireframePass->sinks.color, m_wireframePass->sinks.depth, m_wireframePass->isMsaa && m_numSamples > 1);
    SetViewportAndScissor(context, swapChain.GetDesc(), viewport);
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
                                 const ParticleRenderState& state,
                                 const Viewport& viewport)
{
    NC_PROFILE_SCOPE("PassBackend::RenderParticle", ProfileCategory::Rendering);
    if (state.particleData.instances.empty() || !m_particlePass)
    {
        return;
    }

    m_finalColorTarget = m_particlePass->sinks.color;
    BindRenderTarget(context, swapChain, *m_perPassResourceSignature, m_particlePass->sinks.color, m_particlePass->sinks.depth, m_particlePass->isMsaa && m_numSamples > 1);
    SetViewportAndScissor(context, swapChain.GetDesc(), viewport);
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
                                    ISwapChain& swapChain)
{
    NC_PROFILE_SCOPE("PassBackend::RenderPostProcess()", ProfileCategory::Rendering);
    constexpr auto drawAttribs = DrawAttribs{4, DRAW_FLAG_VERIFY_ALL};
    auto& propertyBuffer = m_perPassResourceSignature->GetPostProcessPropertyResource();
    auto& sinkIndexBuffer = m_perPassResourceSignature->GetSinkIndexBufferResource();

    // If MSAA samples are set to be greater than 1 in the config, all PassType::Material, PassType::SkinnedMaterial and PassType::Misc passes are multisampled.
    // These need to be resolved before used by the post process passes.
    ResolveMsaaTextures(context, *m_perPassResourceSignature, m_numSamples);
    context.TransitionShaderResources(&m_perPassResourceSignature->GetResourceBinding());

    for (const auto& pass : m_postProcessPasses)
    {
        if (!pass.anyEnabled) continue;

        m_finalPostProcessTarget = pass.sinks.postProcess;

        // Get the post process resource we are writing to to bind in the next step
        auto& postProcessSinkBuffer = m_perPassResourceSignature->GetPostProcessSinkResource(pass.sinks.postProcess);

        BindPostProcessRenderTarget(context, swapChain, postProcessSinkBuffer, pass.sinks.postProcess);
        ClearPostProcessRenderTarget(context, swapChain, postProcessSinkBuffer, pass.sinks.postProcess);

        // If this post process pass consumes any post process pass as a source, bind that source's shader resource view to the SRB.
        auto hasPostProcessSource = pass.sources.postProcess != NoTarget;
        if (hasPostProcessSource)
        {
            auto& postProcessSourceBuffer = m_perPassResourceSignature->GetPostProcessSinkResource(pass.sources.postProcess);
            postProcessSourceBuffer.Update();
            m_perPassResourceSignature->Commit(context);
        }
        sinkIndexBuffer.Update(context, pass.sources.color, pass.sources.depth, hasPostProcessSource, std::numeric_limits<uint32_t>::max());
        context.SetPipelineState(pass.pso);

        // Bind the property buffer for the instance, if any
        for (auto& instance : pass.instances)
        {
            if (!instance.enabled) continue;
            if (instance.properties.has_value())
            {
                propertyBuffer.Update(context, instance.properties.value());
                m_perPassResourceSignature->Commit(context);
        }
            context.Draw(drawAttribs);
        }
    }
}

void PassBackend::RenderOutputToSwapchain(IDeviceContext& context, ISwapChain& swapChain)
{
    NC_PROFILE_SCOPE("PassBackend::RenderOutputToSwapchain()", ProfileCategory::Rendering);
    constexpr auto drawAttribs = DrawAttribs{4, DRAW_FLAG_VERIFY_ALL};
    auto& sinkIndexBuffer = m_perPassResourceSignature->GetSinkIndexBufferResource();

    // Render final post process pass
    // Bind the swapchain as the render target
    BindRenderTarget(context, swapChain, *m_perPassResourceSignature, m_finalPass->sinks.color, m_finalPass->sinks.depth, false);
    ClearRenderTarget(context, swapChain, *m_perPassResourceSignature, m_finalPass->sinks.color, m_finalPass->sinks.depth, false);

    // This final pass renders the last pass in the chain's sink target to the swapchain. It's either a color target or a post process target,
    // depending on whether the post process passes are enabled or disabled.
    m_finalPass->sources.color.clear();
    auto hasPostProcess = false;
    if (m_finalPostProcessTarget.has_value())  // The last pass in the chain is a post process target
    {
        m_finalPass->sources.postProcess = m_finalPostProcessTarget.value();
        auto& postProcessSourceBuffer = m_perPassResourceSignature->GetPostProcessSinkResource(m_finalPass->sources.postProcess);
        postProcessSourceBuffer.Update();
       m_perPassResourceSignature->Commit(context);
        hasPostProcess = true;
    }
    else // The last pass in the chain is a color target
    {
        m_finalPass->sources.color.push_back(m_finalColorTarget.value());
        hasPostProcess = false;
    }

    sinkIndexBuffer.Update(context, m_finalPass->sources.color, std::vector<uint32_t>(), hasPostProcess, std::numeric_limits<uint32_t>::max());
    context.SetPipelineState(m_finalPass->pso);
    context.Draw(drawAttribs);
}

void PassBackend::MakePassesAndPipelines(IRenderDevice& device,
                             ISwapChain& swapChain,
                             ShaderFactory& shaderFactory,
                             ShaderBindings& shaderBindings,
                             const PassManifest& passManifest)
{
    const auto shaderCache = ShaderCache{shaderFactory};

    // Create the static material passes
    m_staticMaterialPasses.reserve(passManifest.StaticMaterialPassDescs().size());
    for (const auto& passDesc : passManifest.StaticMaterialPassDescs())
    {
        m_staticMaterialPasses.emplace_back(device, shaderCache.Get(passDesc.shaderPaths), shaderBindings, passManifest, passDesc, m_numSamples);
    }

    // Create the skinned material passes
    m_skinnedMaterialPasses.reserve(passManifest.SkinnedMaterialPassDescs().size());
    for (const auto& passDesc : passManifest.SkinnedMaterialPassDescs())
    {
        m_skinnedMaterialPasses.emplace_back(device, shaderCache.Get(passDesc.shaderPaths), shaderBindings, passManifest, passDesc, m_numSamples);
    }

    // Create the sky box pass
    {
        const auto& passDesc = passManifest.SkyboxPassDesc();
        m_skyboxPass = std::make_unique<SkyboxPass>(device, shaderCache.Get(passDesc.shaderPaths), shaderBindings, passManifest, passDesc);
    }

    // Create the wireframe pass
    {
        const auto& passDesc = passManifest.WireframePassDesc();
        m_wireframePass = std::make_unique<WireframePass>(device, shaderCache.Get(passDesc.shaderPaths), shaderBindings, passManifest, passDesc, m_numSamples);
    }

    // Create the particle pass
    {
        const auto& passDesc = passManifest.ParticlePassDesc();
        m_particlePass = std::make_unique<ParticlePass>(device, shaderCache.Get(passDesc.shaderPaths), shaderBindings, passManifest, passDesc, m_numSamples);
        m_finalColorTarget = m_particlePass->sinks.color;
    }

    // Create the post process passes
    m_postProcessPasses.reserve(passManifest.PostProcessPassDescs().size());
    for (const auto& passDesc : passManifest.PostProcessPassDescs())
    {
        m_postProcessPasses.emplace_back(device, swapChain, shaderCache.Get(passDesc.shaderPaths), shaderBindings, passManifest, passDesc);
    }
    m_finalPostProcessTarget = m_postProcessPasses.back().sinks.postProcess;

    // Create a hardcoded dummy post process pass that always pipes the result of the last rendered render target to the swapchain
    auto shaderPaths = ShaderPaths{shader::PPEndPixel, shader::PostProcessVertex};
    auto name = "Final Pass";
    m_finalPass = std::make_unique<PostProcessPass>(device, swapChain, shaderCache.Get(shaderPaths), shaderBindings, passManifest, PassDesc
    {
        .flag = 0, // No flag used for this pass, it is not treated like the others (not in the collection)
        .name = name,
        .type = PassType::PostProcess,
        .shaderPaths = shaderPaths,
        .colorSink = ColorTarget::Swapchain,
        .depthSink = DepthTarget::DepthStencil,
        .useDepthTest = false
    }, true);
    m_finalPass->sources.postProcess = m_finalPostProcessTarget.value();
}
} // namespace nc::graphics
