#include "UniformBuffer.h"

#include "ncutility/NcError.h"

#include "GraphicsUtilities.h"

#include <cstring>

namespace nc::graphics
{
UniformBuffer::UniformBuffer(Diligent::IDeviceContext& context,
                             Diligent::IRenderDevice& device,
                             size_t size,
                             std::string_view name)
    : m_size{size}
{
    Diligent::CreateUniformBuffer(
        &device,
        size,
        name.data(),
        &m_buffer
    );

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
}

void UniformBuffer::Update(Diligent::IDeviceContext& context, const void* src, size_t size)
{
    NC_ASSERT(size == m_size, "Unexpected UniformBuffer size");
    void* mapped = nullptr;
    context.MapBuffer(
        m_buffer.RawPtr(),
        Diligent::MAP_WRITE,
        Diligent::MAP_FLAG_DISCARD,
        mapped
    );

    std::memcpy(mapped, src, size);
    context.UnmapBuffer(m_buffer.RawPtr(), Diligent::MAP_WRITE);
}
} // namespace nc::graphics
