#pragma once

#include "base/DynamicUniformBuffer.h"

#include "Graphics/GraphicsEngine/interface/RenderDevice.h"
#include "Graphics/GraphicsEngine/interface/DeviceContext.h"

namespace nc::graphics
{
struct EnvironmentRenderState;
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
                    const LightRenderState& lightRenderState,
                    const EnvironmentRenderState& environmentRenderState);

        auto GetShaderVariable() -> Diligent::IShaderResourceVariable&
        {
            return *m_variable;
        }

    private:
        DynamicUniformBuffer m_buffer;
        Diligent::IShaderResourceVariable* m_variable;
        float m_elapsedSeconds;
};
} // namespace nc::graphics
