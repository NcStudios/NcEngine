#include "IndexBuffer.h"
#include "graphics/vulkan/Base.h"
#include "graphics/vulkan/Commands.h"
#include "graphics/d3dresource/GraphicsResourceManager.h"
#include "graphics/Graphics2.h"

namespace nc::graphics::vulkan
{
    IndexBuffer::IndexBuffer(std::vector<uint32_t> indices)
    : m_base { d3dresource::GraphicsResourceManager::GetGraphics2()->GetBasePtr() },
      m_memoryIndex { 0 },
      m_size { static_cast<uint32_t>(sizeof(uint32_t) * indices.size()) },
      m_indexBuffer { nullptr },
      m_indices { std::move(indices) }
    {
    }

    IndexBuffer::~IndexBuffer()
    {
        if (m_indexBuffer)
        {
            m_base->DestroyBuffer(m_memoryIndex);
        }
    }

    void IndexBuffer::Bind()
    {
        // Create staging buffer (lives on CPU).
        vk::Buffer stagingBuffer;
        auto stagingBufferMemoryIndex = m_base->CreateBuffer(m_size, vk::BufferUsageFlagBits::eTransferSrc, true, &stagingBuffer);

        // Map the vertices onto the staging buffer.
        m_base->MapMemory(stagingBufferMemoryIndex, m_indices, m_size);

        // Create vertex buffer (lives on GPU).
        m_memoryIndex = m_base->CreateBuffer(m_size, vk::BufferUsageFlagBits::eIndexBuffer | vk::BufferUsageFlagBits::eTransferDst, false, &m_indexBuffer);

        // Copy staging into vertex.
        Commands::SubmitCopyCommandImmediate(*m_base, stagingBuffer, m_indexBuffer, m_size);

        // Destroy the staging buffer.
        m_base->DestroyBuffer(stagingBufferMemoryIndex);
    }

    const vk::Buffer& IndexBuffer::GetBuffer() const
    {
        return m_indexBuffer;
    }

    const std::vector<uint32_t>& IndexBuffer::GetIndices() const
    {
        return m_indices;
    }

}