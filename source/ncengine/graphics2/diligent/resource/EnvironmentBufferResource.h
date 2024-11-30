#pragma once

#include "base/UniformBuffer.h"

#include "Graphics/GraphicsEngine/interface/RenderDevice.h"
#include "Graphics/GraphicsEngine/interface/DeviceContext.h"

namespace nc::graphics
{
struct CameraRenderState;
struct LightRenderState;

class EnvironmentBufferResource
{
    public:
        static constexpr auto UniformBufferName = "EnvironmentDataUniformBuffer";

        explicit EnvironmentBufferResource(Diligent::IDeviceContext& context,
                                           Diligent::IRenderDevice& device,
                                           Diligent::IShaderResourceVariable& variable);

        void Update(Diligent::IDeviceContext& context,
                    const CameraRenderState& cameraState,
                    const LightRenderState& lightRenderState);

        auto GetShaderVariable() -> Diligent::IShaderResourceVariable&
        {
            return *m_variable;
        }

    private:
        UniformBuffer m_buffer;
        Diligent::IShaderResourceVariable* m_variable;
};
} // namespace nc::graphics
