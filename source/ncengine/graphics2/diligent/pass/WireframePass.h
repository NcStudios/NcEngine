#pragma once

#include "graphics2/frontend/subsystem/WireframeRendererState.h"

#include "Common/interface/RefCntAutoPtr.hpp"
#include "Graphics/GraphicsEngine/interface/RenderDevice.h"
#include "Graphics/GraphicsEngine/interface/DeviceContext.h"

namespace nc::graphics
{
class ShaderBindings;
class ShaderFactory;
class WireframeBufferResource;

class WireframePass
{
    public:
        explicit WireframePass(Diligent::IRenderDevice& device,
                               Diligent::ISwapChain& swapChain,
                               ShaderFactory& shaderFactory,
                               ShaderBindings& shaderBindings);

        void Render(Diligent::IDeviceContext& context,
                    const WireframeRendererRenderState& state);

    private:
        Diligent::RefCntAutoPtr<Diligent::IPipelineState> m_pso;
        WireframeBufferResource* m_buffer;
};
} // namespace nc::graphics
