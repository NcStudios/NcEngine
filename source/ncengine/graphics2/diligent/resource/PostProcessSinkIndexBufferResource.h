#pragma once

#include "base/DynamicUniformBuffer.h"

#include "Graphics/GraphicsEngine/interface/RenderDevice.h"
#include "Graphics/GraphicsEngine/interface/DeviceContext.h"

namespace nc::graphics
{
struct PostProcessSinkIndexBufferResource
{
    public:
        static constexpr auto UniformBufferName = "PostProcessSinkIndexBuffer";

        explicit PostProcessSinkIndexBufferResource(Diligent::IDeviceContext& context,
                                           Diligent::IRenderDevice& device,
                                           Diligent::IShaderResourceVariable& variable);

        void Update(Diligent::IDeviceContext& context, uint32_t colorRenderTargetIndex, uint32_t depthRenderTargetIndex);

        auto GetShaderVariable() -> Diligent::IShaderResourceVariable&
        {
            return *m_variable;
        }
    
    private:
        DynamicUniformBuffer m_buffer;
        Diligent::IShaderResourceVariable* m_variable;
};
} // namespace nc::graphics
