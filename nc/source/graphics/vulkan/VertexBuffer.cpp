#include "VertexBuffer.h"
#include "Base.h"
#include "Commands.h"

namespace nc::graphics::vulkan::vertex
{
    vk::VertexInputBindingDescription GetVertexBindingDescription()
    {
        vk::VertexInputBindingDescription bindingDescription{};
        bindingDescription.setBinding(0);
        bindingDescription.setStride(sizeof(Vertex));
        bindingDescription.setInputRate(vk::VertexInputRate::eVertex); // @todo Change to eInstance for instance data
        return bindingDescription;
    }

    std::array<vk::VertexInputAttributeDescription, 2> GetVertexAttributeDescriptions()
    {
        std::array<vk::VertexInputAttributeDescription, 2> attributeDescriptions{};
        attributeDescriptions[0].setBinding(0);
        attributeDescriptions[0].setLocation(0);
        attributeDescriptions[0].setFormat(vk::Format::eR32G32Sfloat);
        attributeDescriptions[0].setOffset(offsetof(Vertex, Position));

        attributeDescriptions[1].setBinding(0);
        attributeDescriptions[1].setLocation(1);
        attributeDescriptions[1].setFormat(vk::Format::eR32G32B32Sfloat);
        attributeDescriptions[1].setOffset(offsetof(Vertex, Color));
        return attributeDescriptions;
    }
}

namespace nc::graphics::vulkan
{
    VertexBuffer::VertexBuffer(Base& base, Commands& commands, std::vector<vertex::Vertex> vertices)
    : m_base { base },
      m_commands { commands },
      m_id { 0 },
      m_size { 0 },
      m_vertexBuffer { nullptr },
      m_stagingBufferId { 0 },
      m_stagingBuffer { nullptr },
      m_vertices { std::move(vertices) }
    {
        m_size = sizeof(m_vertices[0]) * m_vertices.size();

        // Create staging buffer (lives on CPU).
        m_stagingBufferId = m_base.CreateBuffer(m_size, vk::BufferUsageFlagBits::eTransferSrc, true, &m_stagingBuffer);

        // Map the vertices onto the staging buffer.
        m_base.MapMemory(m_stagingBufferId, m_vertices, m_size);

        // Create vertex buffer (lives on GPU).
        m_id = m_base.CreateBuffer(m_size, vk::BufferUsageFlagBits::eVertexBuffer | vk::BufferUsageFlagBits::eTransferDst, false, &m_vertexBuffer);

        // Copy the staging buffer into the vertex buffer.
        Bind();
    }

    void VertexBuffer::Bind()
    {
        // Copy staging into vertex.
        m_commands.SubmitCopyCommandImmediate(m_base, m_stagingBuffer, m_vertexBuffer, m_size);

        // Destroy the staging buffer.
        m_base.DestroyBuffer(m_stagingBufferId);
    }

    uint32_t VertexBuffer::GetSize() const
    {
        return m_vertices.size();
    }

    const vk::Buffer& VertexBuffer::GetBuffer() const
    {
        return m_vertexBuffer;
    }

    uint32_t VertexBuffer::GetId() const
    {
        return m_id;
    }

    const std::vector<vertex::Vertex>& VertexBuffer::GetVertices() const
    {
        return m_vertices;
    }

}