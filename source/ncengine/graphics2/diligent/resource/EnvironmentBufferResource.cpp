#include "EnvironmentBufferResource.h"
#include "graphics2/frontend/subsystem/CameraRenderState.h"
#include "graphics2/frontend/subsystem/EnvironmentRenderState.h"
#include "graphics2/frontend/subsystem/LightRenderState.h"
#include "ncengine/graphics/Environment.h"

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
                                       const LightRenderState& lightRenderState,
                                       const EnvironmentRenderState& environmentRenderState)
{
    const auto data = GlobalEnvironmentData{
        .cameraViewProjection = cameraState.viewProjection,
        .cameraInvProjection = cameraState.invProjection,
        .primaryColor = environmentRenderState.primaryColor,
        .secondaryColor = environmentRenderState.secondaryColor,
        .tertiaryColor = environmentRenderState.tertiaryColor,
        .cameraPosition = cameraState.position,
        .lightCount = static_cast<uint32_t>(lightRenderState.lights.size()),
        .nearClip = cameraState.nearClip,
        .farClip = cameraState.farClip,
        .skyboxIndex = environmentRenderState.skyboxIndex,
        .useSkybox = environmentRenderState.useSkybox,
        .useColorOverride = environmentRenderState.useColorOverride
    };

    m_buffer.Write(context, data);
}
} // namespace nc::graphics
