#include "MeshRendererBufferResource.h"
#include "graphics2/frontend/subsystem/MeshRendererRenderState.h"

#include "ncutility/NcError.h"

#include "GraphicsUtilities.h"
#include "MapHelper.hpp"

namespace nc::graphics
{
MeshRendererBufferResource::MeshRendererBufferResource(Diligent::IShaderResourceVariable& variable,
                                                       Diligent::IRenderDevice& device,
                                                       uint32_t maxMeshRenderers)
    : m_variable{&variable},
      m_maxMeshRenderers{maxMeshRenderers}
{
    auto structuredBufferDesc = Diligent::BufferDesc
    {
        StructuredBufferName,                          // Name
        sizeof(MeshRendererData) * m_maxMeshRenderers, // Size,
        Diligent::BIND_SHADER_RESOURCE,                // BindFlags
        Diligent::USAGE_DYNAMIC,                       // Usage
        Diligent::CPU_ACCESS_WRITE,                    // CPU_ACCESS_FLAGS
        Diligent::BUFFER_MODE_STRUCTURED,              // Mode
        sizeof(MeshRendererData),                      // ElementByteStride
    };

    device.CreateBuffer(structuredBufferDesc, nullptr, &m_structuredBuffer);

    if (!m_structuredBuffer)
    {
        throw NcError("Failed to create structured buffer");
    }
    
    m_variable->Set(m_structuredBuffer);
}

auto MeshRendererBufferResource::MakeResourceDesc(std::string_view variableName, uint32_t maxMeshRenderers) -> Diligent::PipelineResourceDesc
{
    return Diligent::PipelineResourceDesc{
        Diligent::SHADER_TYPE::SHADER_TYPE_VS_PS,
        variableName.data(),
        maxMeshRenderers,
        Diligent::SHADER_RESOURCE_TYPE_BUFFER_SRV,
        Diligent::SHADER_RESOURCE_VARIABLE_TYPE_DYNAMIC
    };
}

void MeshRendererBufferResource::Update(const MeshRendererRenderState renderState, Diligent::IDeviceContext& context)
{
    auto modelMatrices = Diligent::MapHelper<DirectX::XMFLOAT4X4>{
        &context,
        m_structuredBuffer,
        Diligent::MAP_WRITE,
        Diligent::MAP_FLAG_DISCARD
    };

    for (size_t index = 0u; auto& modelMatrix : renderState.modelMatrices)
    {
        DirectX::XMStoreFloat4x4(&modelMatrices[index], modelMatrix);
    }
}
} // namespace nc::graphics
