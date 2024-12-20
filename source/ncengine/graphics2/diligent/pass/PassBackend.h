#pragma once

#include "graphics2/frontend/subsystem/MeshRenderState.h"
#include "graphics2/frontend/subsystem/particle/ParticleRenderState.h"
#include "graphics2/frontend/subsystem/PostProcessState.h"
#include "MaterialPass.h"
#include "ParticlePass.h"
#include "PassManifest.h"
#include "PostProcessPass.h"
#include "WireframePass.h"

#include "Graphics/GraphicsEngine/interface/DeviceContext.h"

#include <memory>
#include <vector>

namespace nc::graphics
{
class PerFrameResourceSignature;
class PerPassResourceSignature;
class PostProcessPropertyBufferResource;

class PassBackend
{
    public:
        explicit PassBackend(Diligent::IRenderDevice& device,
                             Diligent::ISwapChain& swapChain,
                             ShaderFactory& shaderFactory,
                             ShaderBindings& shaderBindings,
                             const PassManifest& passManifest);

        void Update(const PostProcessState& postProcessState);

        void RenderMaterial(Diligent::IDeviceContext& context,
                            Diligent::ISwapChain& swapChain,
                            PerPassResourceSignature& perPassResourceSignature,
                            const std::vector<std::vector<Batch>>& staticPassBatches,
                            const std::vector<std::vector<Batch>>& skinnedPassBatches);

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
                               PerPassResourceSignature& perPassResourceSignature,
                               PerFrameResourceSignature& perFrameResourceSignature);

        auto FinalColorTarget() const -> uint32_t;

    private:
        std::vector<MaterialPass> m_staticMaterialPasses;
        std::vector<MaterialPass> m_skinnedMaterialPasses;
        std::unique_ptr<WireframePass> m_wireframePass;
        std::unique_ptr<ParticlePass> m_particlePass;
        std::vector<PostProcessPass> m_postProcessPasses;
        std::unique_ptr<PostProcessPass> m_finalPass;
};
} // namespace nc::graphics
