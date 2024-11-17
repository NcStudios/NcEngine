#include "GlobalEnvironmentResource.h"
#include "graphics2/frontend/subsystem/CameraRenderState.h"
#include "graphics2/frontend/subsystem/LightRenderState.h"

#include "ncutility/NcError.h"

#include "GraphicsUtilities.h"
#include "MapHelper.hpp"

namespace nc::graphics
{
GlobalEnvironmentResource::GlobalEnvironmentResource(Diligent::IDeviceContext& context,
                                                     Diligent::IRenderDevice& device,
                                                     Diligent::IShaderResourceVariable& variable)
    : m_variable{&variable}
{
    Diligent::CreateUniformBuffer(
        &device,
        sizeof(GlobalEnvironmentData),
        UniformBufferName,
        &m_uniformBuffer
    );

    if (!m_uniformBuffer)
    {
        throw NcError("Failed to create uniform buffer");
    }

    const auto barrier = Diligent::StateTransitionDesc{
        m_uniformBuffer,
        Diligent::RESOURCE_STATE_UNKNOWN,
        Diligent::RESOURCE_STATE_CONSTANT_BUFFER,
        Diligent::STATE_TRANSITION_FLAG_UPDATE_STATE
    };

    context.TransitionResourceStates(1, &barrier);
    m_variable->Set(m_uniformBuffer);
}

void GlobalEnvironmentResource::Update(Diligent::IDeviceContext& context,
                                       const CameraRenderState cameraState,
                                       const LightRenderState lightRenderState)
{
    auto cbConstants = Diligent::MapHelper<GlobalEnvironmentData>{
        &context,
        m_uniformBuffer,
        Diligent::MAP_WRITE,
        Diligent::MAP_FLAG_DISCARD
    };

    cbConstants->cameraViewProjection = cameraState.viewProjection;
    cbConstants->cameraPosition = cameraState.position;
    cbConstants->dirLightsCount = static_cast<uint32_t>(lightRenderState.directionalLights.size());
    cbConstants->pointLightsCount = static_cast<uint32_t>(lightRenderState.pointLights.size());
    cbConstants->spotLightsCount = static_cast<uint32_t>(lightRenderState.spotLights.size());
}
} // namespace nc::graphics
