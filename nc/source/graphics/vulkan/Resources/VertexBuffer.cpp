#include "VertexBuffer.h"
#include "graphics/vulkan/Base.h"
#include "graphics/vulkan/Commands.h"
#include "graphics/d3dresource/GraphicsResourceManager.h"
#include "graphics/Graphics2.h"

namespace nc::graphics::vulkan
{
    vk::VertexInputBindingDescription GetVertexBindingDescription()
    {
        vk::VertexInputBindingDescription bindingDescription{};
        bindingDescription.setBinding(0);
        bindingDescription.setStride(sizeof(Vertex));
        bindingDescription.setInputRate(vk::VertexInputRate::eVertex); // @todo Change to eInstance for instance data
        return bindingDescription;
    }

    std::array<vk::VertexInputAttributeDescription, 5> GetVertexAttributeDescriptions()
    {
        std::array<vk::VertexInputAttributeDescription, 5> attributeDescriptions{};
        attributeDescriptions[0].setBinding(0);
        attributeDescriptions[0].setLocation(0);
        attributeDescriptions[0].setFormat(vk::Format::eR32G32B32Sfloat);
        attributeDescriptions[0].setOffset(offsetof(Vertex, Position));

        attributeDescriptions[1].setBinding(0);
        attributeDescriptions[1].setLocation(1);
        attributeDescriptions[1].setFormat(vk::Format::eR32G32B32Sfloat);
        attributeDescriptions[1].setOffset(offsetof(Vertex, Normal));

        attributeDescriptions[2].setBinding(0);
        attributeDescriptions[2].setLocation(2);
        attributeDescriptions[2].setFormat(vk::Format::eR32G32Sfloat);
        attributeDescriptions[2].setOffset(offsetof(Vertex, UV));

        attributeDescriptions[3].setBinding(0);
        attributeDescriptions[3].setLocation(3);
        attributeDescriptions[3].setFormat(vk::Format::eR32G32B32Sfloat);
        attributeDescriptions[3].setOffset(offsetof(Vertex, Tangent));

        attributeDescriptions[4].setBinding(0);
        attributeDescriptions[4].setLocation(4);
        attributeDescriptions[4].setFormat(vk::Format::eR32G32B32Sfloat);
        attributeDescriptions[4].setOffset(offsetof(Vertex, Bitangent));
        return attributeDescriptions;
    }

    VertexBuffer::VertexBuffer(std::vector<Vertex> vertices)
    : m_base { d3dresource::GraphicsResourceManager::GetGraphics2()->GetBasePtr() },
      m_memoryIndex { 0 },
      m_size { static_cast<uint32_t>(sizeof(vertices[0]) * vertices.size()) },
      m_vertexBuffer { nullptr },
      m_vertices {std::move(vertices)}
    {
    }

    void VertexBuffer::Bind()
    {
        // Create staging buffer (lives on CPU).
        vk::Buffer stagingBuffer;
        auto stagingBufferMemoryIndex =  m_base->CreateBuffer(m_size, vk::BufferUsageFlagBits::eTransferSrc, true, &stagingBuffer);

        // Map the vertices onto the staging buffer.
        m_base->MapMemory(stagingBufferMemoryIndex, m_vertices, m_size);

        // Create vertex buffer (lives on GPU).
        m_memoryIndex = m_base->CreateBuffer(m_size, vk::BufferUsageFlagBits::eVertexBuffer | vk::BufferUsageFlagBits::eTransferDst, false, &m_vertexBuffer);

        // Copy staging into vertex.
        vulkan::Commands::SubmitCopyCommandImmediate(*m_base, stagingBuffer, m_vertexBuffer, m_size);

        // Destroy the staging buffer.
        m_base->DestroyBuffer(stagingBufferMemoryIndex);
    }

    VertexBuffer::~VertexBuffer()
    {
        if (m_vertexBuffer)
        {
            m_base->DestroyBuffer(m_memoryIndex);
        }
    }

    const vk::Buffer& VertexBuffer::GetBuffer() const
    {
        return m_vertexBuffer;
    }
}