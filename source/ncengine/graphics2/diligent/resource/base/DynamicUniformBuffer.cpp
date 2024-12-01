#include "DynamicUniformBuffer.h"

#include "ncutility/NcError.h"

#include <cstring>

namespace nc::graphics
{
DynamicUniformBuffer::DynamicUniformBuffer(Diligent::IDeviceContext& context,
                                           Diligent::IRenderDevice& device,
                                           const void* data,
                                           size_t size,
                                           std::string_view name)
    : m_size{size}
{
    const auto desc = Diligent::BufferDesc{
        name.data(),
        size,
        Diligent::BIND_UNIFORM_BUFFER,
        Diligent::USAGE_DYNAMIC,
        Diligent::CPU_ACCESS_WRITE
    };

    device.CreateBuffer(desc, nullptr, &m_buffer);
    if (!m_buffer)
    {
        throw NcError("Failed to create uniform buffer");
    }

    const auto barrier = Diligent::StateTransitionDesc{
        m_buffer,
        Diligent::RESOURCE_STATE_UNKNOWN,
        Diligent::RESOURCE_STATE_CONSTANT_BUFFER,
        Diligent::STATE_TRANSITION_FLAG_UPDATE_STATE
    };

    context.TransitionResourceStates(1, &barrier);
    Write(context, data, size); // initial data must be null for dynamic buffers, not sure why
}

void DynamicUniformBuffer::Write(Diligent::IDeviceContext& context, const void* src, size_t size)
{
    NC_ASSERT(size == m_size, "Source size does not match DynamicUniformBuffer size");
    auto mapped = Map(context);
    std::memcpy(mapped, src, size);
    Unmap(context);
}

auto DynamicUniformBuffer::Map(Diligent::IDeviceContext& context) -> void*
{
    void* mapped = nullptr;
    context.MapBuffer(
        m_buffer.RawPtr(),
        Diligent::MAP_WRITE,
        Diligent::MAP_FLAG_DISCARD,
        mapped
    );

    return mapped;
}

void DynamicUniformBuffer::Unmap(Diligent::IDeviceContext& context)
{
    context.UnmapBuffer(m_buffer.RawPtr(), Diligent::MAP_WRITE);
}
} // namespace nc::graphics
