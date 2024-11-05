#include "MaterialDataBufferResource.h"
#include "graphics2/ShaderTypes.h"

#include "ncutility/NcError.h"

namespace nc::graphics
{
MaterialDataBufferResource::MaterialDataBufferResource(Diligent::IDeviceContext& context,
                                                       Diligent::IRenderDevice& device,
                                                       Diligent::IShaderResourceVariable& variable,
                                                       uint32_t initialInstanceCountHint)
    : m_variable{&variable}
{
    const auto dummy = std::vector<MaterialData>(initialInstanceCountHint);
    CreateBuffer(context, device, dummy);
}

auto MaterialDataBufferResource::MakeResourceDesc(std::string_view variableName) -> Diligent::PipelineResourceDesc
{
    return Diligent::PipelineResourceDesc{
        Diligent::SHADER_TYPE::SHADER_TYPE_PIXEL,
        variableName.data(),
        1,
        Diligent::SHADER_RESOURCE_TYPE_BUFFER_SRV,
        Diligent::SHADER_RESOURCE_VARIABLE_TYPE_MUTABLE
    };
}

void MaterialDataBufferResource::CreateBuffer(Diligent::IDeviceContext& context,
                                              Diligent::IRenderDevice& device,
                                              std::span<const MaterialData> data)
{
    constexpr auto elementSize = static_cast<uint32_t>(sizeof(MaterialData));
    m_bufferElementCount = static_cast<uint32_t>(data.size());
    const auto bufferSize = elementSize * m_bufferElementCount;
    const auto bufferDesc = Diligent::BufferDesc{
        "MaterialDataBuffer",
        bufferSize,
        Diligent::BIND_SHADER_RESOURCE,
        Diligent::USAGE_DEFAULT,
        Diligent::CPU_ACCESS_NONE,
        Diligent::BUFFER_MODE_STRUCTURED,
        elementSize
    };

    auto bufferData = Diligent::BufferData{data.data(), bufferSize};
    m_buffer.Release();
    device.CreateBuffer(bufferDesc, &bufferData, &m_buffer);
    if (!m_buffer)
    {
        throw nc::NcError{fmt::format("Failed to create buffer '{}'", bufferDesc.Name)};
    }

    const auto barrier = Diligent::StateTransitionDesc(
        m_buffer,
        Diligent::RESOURCE_STATE_UNKNOWN,
        Diligent::RESOURCE_STATE_SHADER_RESOURCE,
        Diligent::STATE_TRANSITION_FLAG_UPDATE_STATE
    );

    context.TransitionResourceStates(1, &barrier);
}

void MaterialDataBufferResource::Update(const MaterialDataUpdateInfo& updateInfo,
                                        Diligent::IDeviceContext& context,
                                        Diligent::IRenderDevice& device)
{
    if (updateInfo.instances.size() > m_bufferElementCount)
    {
        CreateBuffer(context, device, updateInfo.instances);
        return;
    }

    constexpr auto elementSize = sizeof(MaterialData);
    for (const auto& [offset, count] : updateInfo.dirtyRanges)
    {
        const auto source = &updateInfo.instances[offset];
        context.UpdateBuffer(
            m_buffer,
            offset * elementSize,
            count * elementSize,
            source,
            Diligent::RESOURCE_STATE_TRANSITION_MODE_TRANSITION
        );
    }

    m_view = m_buffer->GetDefaultView(Diligent::BUFFER_VIEW_SHADER_RESOURCE);
    m_variable->Set(m_view, Diligent::SET_SHADER_RESOURCE_FLAG_NONE);
}
} // namespace nc::graphics
