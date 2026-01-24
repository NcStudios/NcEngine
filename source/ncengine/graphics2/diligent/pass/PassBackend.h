#pragma once

#include "graphics2/frontend/subsystem/EnvironmentRenderState.h"
#include "graphics2/frontend/subsystem/LightRenderState.h"
#include "graphics2/frontend/subsystem/MeshRenderState.h"
#include "graphics2/frontend/subsystem/particle/ParticleRenderState.h"
#include "graphics2/frontend/subsystem/PostProcessState.h"
#include "MaterialPass.h"
#include "ParticlePass.h"
#include "PassManifest.h"
#include "PostProcessPass.h"
#include "SkyboxPass.h"
#include "WireframePass.h"
#include "ncengine/scene/NcScene.h"
#include "ncengine/window/WindowTypes.h"

#include "Graphics/GraphicsEngine/interface/DeviceContext.h"

#include <memory>
#include <vector>

namespace nc
{
namespace config
{
struct GraphicsSettings;
struct MemorySettings;
}
namespace graphics
{
class PerFrameResourceSignature;
class PerPassResourceSignature;
class PostProcessPropertyBufferResource;

class PassBackend
{
    public:
        explicit PassBackend(Diligent::IRenderDevice& device,
                             Diligent::IDeviceContext& context,
                             Diligent::ISwapChain& swapChain,
                             ShaderFactory& shaderFactory,
                             ShaderBindings& shaderBindings,
                             const PassManifest& passManifest,
                             const config::GraphicsSettings& graphicsSettings,
                             const config::MemorySettings& memorySettings,
                             uint32_t numSamples = 1u);

        void Update(Diligent::IRenderDevice& device,
                    Diligent::IDeviceContext& context,
                    const PostProcessState& postProcessState);

        void OnBeforeSceneLoad(const Scene& sceneToLoad);

        void RenderShadowPass(Diligent::IDeviceContext& context,
                              const MaterialPass& staticPass,
                              const MaterialPass& skinnedPass,
                              const std::vector<Batch>& staticBatches,
                              const std::vector<Batch>& skinnedBatches,
                              const std::span<const LightData>& lights);

        void RenderSkybox(Diligent::IDeviceContext& context,
                          Diligent::ISwapChain& swapChain,
                          const nc::graphics::EnvironmentRenderState& environmentRenderState,
                          const Viewport& viewport);

        void RenderMaterial(Diligent::IDeviceContext& context,
                            Diligent::ISwapChain& swapChain,
                            const std::vector<std::vector<Batch>>& staticPassBatches,
                            const std::vector<std::vector<Batch>>& skinnedPassBatches,
                            const std::span<const LightData>& lights,
                            const Viewport& viewport);

        void RenderWireframe(Diligent::IDeviceContext& context,
                             Diligent::ISwapChain& swapChain,
                             const WireframeRendererRenderState& state,
                             const Viewport& viewport);

        void RenderParticle(Diligent::IDeviceContext& context,
                            Diligent::ISwapChain& swapChain,
                            const ParticleRenderState& state,
                            const Viewport& viewport);

        void RenderPostProcess(Diligent::IDeviceContext& context,
                               Diligent::ISwapChain& swapChain);

        void RenderOutputToSwapchain(Diligent::IDeviceContext& context,
                                     Diligent::ISwapChain& swapChain);

    private:
        void MakePassesAndPipelines(Diligent::IRenderDevice& device,
                              Diligent::ISwapChain& swapChain,
                              ShaderFactory& shaderFactory,
                              ShaderBindings& shaderBindings,
                              const PassManifest& passManifest);

        std::vector<MaterialPass> m_staticMaterialPasses;
        std::vector<MaterialPass> m_skinnedMaterialPasses;
        std::unique_ptr<SkyboxPass> m_skyboxPass;
        std::unique_ptr<WireframePass> m_wireframePass;
        std::unique_ptr<ParticlePass> m_particlePass;
        std::vector<PostProcessPass> m_postProcessPasses;
        std::unique_ptr<PostProcessPass> m_finalPass;
        uint32_t m_numSamples;
        std::optional<uint32_t> m_finalColorTarget;
        std::optional<uint32_t> m_finalPostProcessTarget;
        PerPassResourceSignature* m_perPassResourceSignature;
        bool m_castsShadows = false;
        uint32_t m_shadowMapResX = 512;
        uint32_t m_maxPointLights = 10;
        uint32_t m_maxDirectionalLights = 10;
        uint32_t m_maxSpotLights = 10;
        uint32_t m_pointSinksToCreate = 0u;
        uint32_t m_uniSinksToCreate = 0u;
};
} // namespace graphics
} // namespace nc
