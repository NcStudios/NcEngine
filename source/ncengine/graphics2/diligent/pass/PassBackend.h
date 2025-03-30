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

        void Update(const PostProcessState& postProcessState);

        void RenderShadowPass(Diligent::IDeviceContext& context,
                              PerPassResourceSignature& perPassResourceSignature,
                              const MaterialPass& staticPass,
                              const MaterialPass& skinnedPass,
                              const std::vector<Batch>& staticBatches,
                              const std::vector<Batch>& skinnedBatches,
                              const std::span<const LightData>& lights);

        void RenderSkybox(Diligent::IDeviceContext& context,
                          Diligent::ISwapChain& swapChain,
                          PerPassResourceSignature& perPassResourceSignature,
                          const nc::graphics::EnvironmentRenderState& environmentRenderState);

        void RenderMaterial(Diligent::IDeviceContext& context,
                            Diligent::ISwapChain& swapChain,
                            PerPassResourceSignature& perPassResourceSignature,
                            const std::vector<std::vector<Batch>>& staticPassBatches,
                            const std::vector<std::vector<Batch>>& skinnedPassBatches,
                            const std::span<const LightData>& lights);

        void RenderWireframe(Diligent::IDeviceContext& context,
                             Diligent::ISwapChain& swapChain,
                             PerPassResourceSignature& perPassResourceSignature,
                             const WireframeRendererRenderState& state);

        void RenderParticle(Diligent::IDeviceContext& context,
                            Diligent::ISwapChain& swapChain,
                            PerPassResourceSignature& perPassResourceSignature,
                            const ParticleRenderState& state);

        void RenderPostProcess(Diligent::IDeviceContext& context,
                               Diligent::ISwapChain& swapChain,
                               PerPassResourceSignature& perPassResourceSignature);

        void RenderOutputToSwapchain(Diligent::IDeviceContext& context,
                                     Diligent::ISwapChain& swapChain,
                                     PerPassResourceSignature& perPassResourceSignature);

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
};
} // namespace graphics
} // namespace nc
