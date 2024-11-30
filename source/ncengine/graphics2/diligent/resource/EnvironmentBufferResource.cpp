#include "EnvironmentBufferResource.h"
#include "graphics2/frontend/subsystem/CameraRenderState.h"
#include "graphics2/frontend/subsystem/LightRenderState.h"

#include "ncutility/NcError.h"

namespace nc::graphics
{
EnvironmentBufferResource::EnvironmentBufferResource(Diligent::IDeviceContext& context,
                                                     Diligent::IRenderDevice& device,
                                                     Diligent::IShaderResourceVariable& variable)
    : m_buffer{
        context,
        device,
        GlobalEnvironmentData{},
        UniformBufferName
      },
      m_variable{&variable}
{
    m_variable->Set(&m_buffer.GetBuffer());
}

void EnvironmentBufferResource::Update(Diligent::IDeviceContext& context,
                                       const CameraRenderState& cameraState,
                                       const LightRenderState& lightRenderState)
{
    const auto data = GlobalEnvironmentData{
        .cameraViewProjection = cameraState.viewProjection,
        .cameraPosition = cameraState.position,
        .dirLightsCount = static_cast<uint32_t>(lightRenderState.directionalLights.size()),
        .pointLightsCount = static_cast<uint32_t>(lightRenderState.pointLights.size()),
        .spotLightsCount = static_cast<uint32_t>(lightRenderState.spotLights.size())
    };

    m_buffer.Write(context, data);
}
} // namespace nc::graphics
