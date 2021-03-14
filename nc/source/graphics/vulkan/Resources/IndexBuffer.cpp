#include "IndexBuffer.h"
#include "Base.h"
#include "Commands.h"

namespace nc::graphics::vulkan
{
    IndexBuffer::IndexBuffer(Base& base, Commands& commands, std::vector<uint32_t> indices)
    : m_base { base },
      m_commands { commands },
      m_id { 0 },
      m_size { 0 },
      m_indexBuffer { nullptr },
      m_stagingBufferId { 0 },
      m_stagingBuffer { nullptr },
      m_indices { indices }
    {
        m_size = sizeof(uint32_t) * m_indices.size();

        // Create staging buffer (lives on CPU).
        m_stagingBufferId = m_base.CreateBuffer(m_size, vk::BufferUsageFlagBits::eTransferSrc, true, &m_stagingBuffer);

        // Map the vertices onto the staging buffer.
        m_base.MapMemory(m_stagingBufferId, indices, m_size);

        // Create vertex buffer (lives on GPU).
        m_id = m_base.CreateBuffer(m_size, vk::BufferUsageFlagBits::eIndexBuffer | vk::BufferUsageFlagBits::eTransferDst, false, &m_indexBuffer);

        // Copy the staging buffer into the vertex buffer.
        Bind();
    }

    void IndexBuffer::Bind()
    {
        // Copy staging into vertex.
        m_commands.SubmitCopyCommandImmediate(m_stagingBuffer, m_indexBuffer, m_size);

        // Destroy the staging buffer.
        m_base.DestroyBuffer(m_stagingBufferId);
    }

    uint32_t IndexBuffer::GetSize() const
    {
        return m_indices.size();
    }

    const vk::Buffer& IndexBuffer::GetBuffer() const
    {
        return m_indexBuffer;
    }

    uint32_t IndexBuffer::GetId() const
    {
        return m_id;
    }

    const std::vector<uint32_t>& IndexBuffer::GetIndices() const
    {
        return m_indices;
    }

}