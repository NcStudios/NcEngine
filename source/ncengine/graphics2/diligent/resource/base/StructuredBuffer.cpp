#include "StructuredBuffer.h"

namespace nc::graphics
{
StructuredBufferBase::StructuredBufferBase(std::string_view name,
                                           Diligent::IShaderResourceVariable& variable,
                                           uint32_t maxElementCount,
                                           uint32_t initialElementCount)
    : m_variable{&variable},
      m_buffer{},
      m_name{name},
      m_maxElementCount{maxElementCount},
      m_elementCount{initialElementCount}
{
}

auto StructuredBufferBase::MakeResourceDesc(std::string_view variableName,
                                            Diligent::SHADER_TYPE shaderType) -> Diligent::PipelineResourceDesc
{
    return Diligent::PipelineResourceDesc{
        shaderType,
        variableName.data(),
        1,
        Diligent::SHADER_RESOURCE_TYPE_BUFFER_SRV,
        Diligent::SHADER_RESOURCE_VARIABLE_TYPE_MUTABLE
    };
}

void StructuredBufferBase::Transition(Diligent::IDeviceContext& context,
                                      Diligent::RESOURCE_STATE state)
{
    const auto barrier = Diligent::StateTransitionDesc(
        m_buffer,
        Diligent::RESOURCE_STATE_UNKNOWN,
        state,
        Diligent::STATE_TRANSITION_FLAG_UPDATE_STATE
    );

    context.TransitionResourceStates(1, &barrier);
}

void StructuredBufferBase::Reallocate(Diligent::IRenderDevice& device,
                                      uint32_t elementCount,
                                      uint32_t elementStride,
                                      Diligent::BufferData* bufferData)
{
    NC_ASSERT(elementCount > 0 && elementCount <= m_maxElementCount, "Invalid buffer size.");
    m_elementCount = elementCount;
    const auto bufferSize = elementCount * elementStride;
    const auto bufferDesc = Diligent::BufferDesc{
        m_name.c_str(),
        bufferSize,
        Diligent::BIND_SHADER_RESOURCE,
        Diligent::USAGE_DEFAULT,
        Diligent::CPU_ACCESS_NONE,
        Diligent::BUFFER_MODE_STRUCTURED,
        elementStride
    };

    m_buffer.Release();
    device.CreateBuffer(bufferDesc, bufferData, &m_buffer);
    if (!m_buffer)
    {
        throw nc::NcError{fmt::format("Failed to create buffer '{}'", bufferDesc.Name)};
    }
}

void StructuredBufferBase::SetVariable()
{
    m_variable->Set(
        m_buffer->GetDefaultView(Diligent::BUFFER_VIEW_SHADER_RESOURCE),
        Diligent::SET_SHADER_RESOURCE_FLAG_ALLOW_OVERWRITE
    );
}
} // namespace nc::graphics
