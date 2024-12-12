#pragma once

#include "graphics2/frontend/subsystem/MeshRenderState.h"
#include "graphics2/frontend/subsystem/PostProcessState.h"
#include "MaterialPass.h"
#include "PassManifest.h"
#include "PostProcessPass.h"
#include "WireframePass.h"

#include "Graphics/GraphicsEngine/interface/DeviceContext.h"

#include <vector>

namespace nc::graphics
{
class PostProcessPropertyBufferResource;
class PerPassResourceSignature;

class PassBackend
{
    public:
        explicit PassBackend(Diligent::IRenderDevice& device,
                             Diligent::IDeviceContext& context,
                             Diligent::ISwapChain& swapChain,
                             ShaderFactory& shaderFactory,
                             ShaderBindings& shaderBindings,
                             const PassManifest& passManifest);

        void Update(Diligent::IDeviceContext& context, const PostProcessState& postProcessState);

        void RenderMaterial(Diligent::IDeviceContext& context,
                            Diligent::ISwapChain& swapChain,
                            PerPassResourceSignature& perPassResourceSignature,
                            const std::vector<std::vector<Batch>>& passBatches);

        void RenderWireframe(Diligent::IDeviceContext& context,
                             Diligent::ISwapChain& swapChain,
                             PerPassResourceSignature& perPassResourceSignature,
                             const WireframeRendererRenderState& state);

        void RenderPostProcess(Diligent::IDeviceContext& context,
                               Diligent::ISwapChain& swapChain,
                               PerPassResourceSignature& perPassResourceSignature);

    private:
        std::vector<MaterialPass> m_materialPasses;
        std::unique_ptr<WireframePass> m_wireframePass;
        std::vector<PostProcessPass> m_postProcessPasses;
        std::unique_ptr<PostProcessPass> m_finalPass;
};
} // namespace nc::graphics
