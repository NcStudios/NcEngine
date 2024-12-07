#pragma once

#include "graphics2/frontend/subsystem/MeshRenderState.h"
#include "graphics2/frontend/subsystem/PostProcessState.h"
#include "MaterialPass.h"
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
                             ShaderBindings& shaderBindings);

        void Update(Diligent::IDeviceContext& context, const PostProcessState& postProcessState);

        void RenderMaterial(Diligent::IDeviceContext& context,
                            Diligent::ISwapChain& swapChain,
                            PerPassResourceSignature& perPassResourceSignature,
                            const std::vector<std::vector<Batch>>& passBatches);

        void RenderWireframe(Diligent::IDeviceContext& context,
                             const WireframeRendererRenderState& state);

        void RenderPostProcess(Diligent::IDeviceContext& context,
                               Diligent::ISwapChain& swapChain,
                               PerPassResourceSignature& perPassResourceSignature,
                               PostProcessPropertyBufferResource& resource);

    private:
        std::vector<MaterialPass> m_materialPasses;
        std::vector<PostProcessPass> m_postProcessPasses;
        WireframePass m_wireframePass;
        uint32_t m_lastColorRenderTargetIndex;
        uint32_t m_lastDepthRenderTargetIndex;
};
} // namespace nc::graphics
