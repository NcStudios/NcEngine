#include "MeshRendererBufferResource.h"
#include "graphics2/frontend/subsystem/MeshRendererRenderState.h"

#include "ncutility/NcError.h"

#include "GraphicsUtilities.h"
#include "MapHelper.hpp"

namespace nc::graphics
{
MeshRendererBufferResource::MeshRendererBufferResource(Diligent::IDeviceContext& context,
                                                       Diligent::IRenderDevice& device,
                                                       Diligent::IShaderResourceVariable& variable,
                                                       uint32_t maxMeshRenderers)
    : m_variable{&variable},
      m_maxMeshRenderers{maxMeshRenderers}
{
    Diligent::BufferDesc structuredBufferDesc;
    structuredBufferDesc.Name = StructuredBufferName;
    structuredBufferDesc.Mode = Diligent::BUFFER_MODE_STRUCTURED;
    structuredBufferDesc.Usage = Diligent::USAGE_DEFAULT;
    structuredBufferDesc.BindFlags = Diligent::BIND_SHADER_RESOURCE;
    structuredBufferDesc.Size = sizeof(DirectX::XMMATRIX) * m_maxMeshRenderers;
    structuredBufferDesc.ElementByteStride = sizeof(DirectX::XMMATRIX);
    device.CreateBuffer(structuredBufferDesc, nullptr, &m_structuredBuffer);

    if (!m_structuredBuffer)
    {
        throw NcError("Failed to create structured buffer");
    }

    const auto barrier = Diligent::StateTransitionDesc{
        m_structuredBuffer,
        Diligent::RESOURCE_STATE_UNKNOWN,
        Diligent::RESOURCE_STATE_SHADER_RESOURCE,
        Diligent::STATE_TRANSITION_FLAG_UPDATE_STATE
    };

    context.TransitionResourceStates(1, &barrier);

    m_variable->Set(m_structuredBuffer->GetDefaultView(Diligent::BUFFER_VIEW_SHADER_RESOURCE));
}

auto MeshRendererBufferResource::MakeResourceDesc(std::string_view variableName, uint32_t ) -> Diligent::PipelineResourceDesc
{
    return Diligent::PipelineResourceDesc{
        Diligent::SHADER_TYPE::SHADER_TYPE_VS_PS,
        variableName.data(),
        1,
        Diligent::SHADER_RESOURCE_TYPE_BUFFER_SRV,
        Diligent::SHADER_RESOURCE_VARIABLE_TYPE_MUTABLE
    };
}

void MeshRendererBufferResource::Update(const MeshRendererRenderState& renderState, Diligent::IDeviceContext& context)
{
    if (renderState.modelMatrices.empty())
    {
        return;
    }

    context.UpdateBuffer(m_structuredBuffer,
                         0u,
                         sizeof(DirectX::XMMATRIX) * renderState.modelMatrices.size(),
                         renderState.modelMatrices.data(),
                         Diligent::RESOURCE_STATE_TRANSITION_MODE_TRANSITION);

    const auto barrier = Diligent::StateTransitionDesc{
        m_structuredBuffer,
        Diligent::RESOURCE_STATE_UNKNOWN,
        Diligent::RESOURCE_STATE_SHADER_RESOURCE,
        Diligent::STATE_TRANSITION_FLAG_UPDATE_STATE
    };

    context.TransitionResourceStates(1, &barrier);
}
} // namespace nc::graphics
