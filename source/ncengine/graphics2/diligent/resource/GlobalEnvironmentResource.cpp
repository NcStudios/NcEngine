#include "GlobalEnvironmentResource.h"
#include "graphics2/frontend/subsystem/CameraRenderState.h"

#include "ncutility/NcError.h"

#include "GraphicsUtilities.h"
#include "MapHelper.hpp"

namespace nc::graphics
{
GlobalEnvironmentResource::GlobalEnvironmentResource(Diligent::IShaderResourceVariable& variable,
                                                     Diligent::IRenderDevice& device)
    : m_variable{&variable}
{
    Diligent::CreateUniformBuffer(
        &device,
        sizeof(GlobalEnvironmentData),
        UniformBufferName,
        &m_uniformBuffer,
        Diligent::USAGE_DYNAMIC
    );

    if (!m_uniformBuffer)
    {
        throw NcError("Failed to create uniform buffer");
    }

    m_variable->Set(m_uniformBuffer);
}

auto GlobalEnvironmentResource::MakeResourceDesc(std::string_view variableName) -> Diligent::PipelineResourceDesc
{
    return Diligent::PipelineResourceDesc{
        Diligent::SHADER_TYPE::SHADER_TYPE_VS_PS,
        variableName.data(),
        Diligent::SHADER_RESOURCE_TYPE_CONSTANT_BUFFER,
        Diligent::SHADER_RESOURCE_VARIABLE_TYPE_DYNAMIC
    };
}

void GlobalEnvironmentResource::Update(const CameraRenderState cameraState,
                                       Diligent::IDeviceContext& context)
{
    auto cbConstants = Diligent::MapHelper<GlobalEnvironmentData>{
        &context,
        m_uniformBuffer,
        Diligent::MAP_WRITE,
        Diligent::MAP_FLAG_DISCARD
    };

    cbConstants->cameraViewProjection = cameraState.viewProjection;
}
} // namespace nc::graphics
