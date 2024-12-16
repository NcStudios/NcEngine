#pragma once

#include "base/DynamicUniformBuffer.h"
#include "graphics2/diligent/pass/PassTypes.h"

#include "Graphics/GraphicsEngine/interface/RenderDevice.h"
#include "Graphics/GraphicsEngine/interface/DeviceContext.h"

#include <array>

namespace nc::graphics
{
struct PerPassInstanceBufferResource
{
    public:
        static constexpr auto UniformBufferName = "PerPassInstanceBuffer";

        explicit PerPassInstanceBufferResource(Diligent::IDeviceContext& context,
                                               Diligent::IRenderDevice& device,
                                               Diligent::IShaderResourceVariable& variable);

        void Update(Diligent::IDeviceContext& context, std::span<const uint32_t> colorSources, std::span<const uint32_t> depthSources, uint32_t spotLightIndex);

        auto GetShaderVariable() -> Diligent::IShaderResourceVariable&
        {
            return *m_variable;
        }
    
    private:
        DynamicUniformBuffer m_buffer;
        Diligent::IShaderResourceVariable* m_variable;
};
} // namespace nc::graphics
