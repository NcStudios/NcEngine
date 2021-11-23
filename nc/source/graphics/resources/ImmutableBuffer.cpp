#include "ImmutableBuffer.h"
#include "graphics/Graphics.h"

namespace nc::graphics
{
    ImmutableBuffer::ImmutableBuffer()
        : m_memoryIndex { 0 },
          m_immutableBuffer { nullptr }
    {
    }

    ImmutableBuffer::ImmutableBuffer(nc::graphics::Graphics* graphics, const std::vector<uint32_t>& data)
        : m_memoryIndex { 0 },
          m_immutableBuffer { nullptr }
    {
        Bind(graphics, data);
    }

    ImmutableBuffer::ImmutableBuffer(nc::graphics::Graphics* graphics, const std::vector<Vertex>& data)
        : m_memoryIndex { 0 },
          m_immutableBuffer { nullptr }
    {
        Bind(graphics, data);
    }

    ImmutableBuffer::ImmutableBuffer(nc::graphics::Graphics* graphics, const SceneData& data)
        : m_memoryIndex { 0 },
          m_immutableBuffer { nullptr }
    {
        Bind(graphics, data);
    }

    ImmutableBuffer::~ImmutableBuffer() noexcept
    {
        if (m_immutableBuffer)
        {
            m_base->DestroyBuffer(m_memoryIndex);
            m_immutableBuffer = nullptr;
        }

        m_base = nullptr;
    }

    ImmutableBuffer::ImmutableBuffer(ImmutableBuffer&& other)
        : m_base{std::exchange(other.m_base, nullptr)},
          m_memoryIndex{std::exchange(other.m_memoryIndex, 0)},
          m_immutableBuffer{std::exchange(other.m_immutableBuffer, nullptr)}
    {
    }

    ImmutableBuffer& ImmutableBuffer::operator = (ImmutableBuffer&& other)
    {
        m_base = std::exchange(other.m_base, nullptr);
        m_memoryIndex = std::exchange(other.m_memoryIndex, 0);
        m_immutableBuffer = std::exchange(other.m_immutableBuffer, nullptr);
        return *this;
    }

    void ImmutableBuffer::Bind(nc::graphics::Graphics* graphics, const std::vector<uint32_t>& data)
    {
        m_base = graphics->GetBasePtr();

        auto size = static_cast<uint32_t>(sizeof(uint32_t) * data.size());

        // Create staging buffer (lives on CPU).
        vk::Buffer stagingBuffer;
        auto stagingBufferMemoryIndex = m_base->CreateBuffer(size, vk::BufferUsageFlagBits::eTransferSrc, vma::MemoryUsage::eCpuOnly, &stagingBuffer);

        // Map the data onto the staging buffer.
        void* mappedData;
        auto* allocator = m_base->GetAllocator();
        auto* allocation = m_base->GetBufferAllocation(stagingBufferMemoryIndex);

        allocator->mapMemory(*allocation, &mappedData);
        memcpy(mappedData, data.data(), size);
        allocator->unmapMemory(*allocation);

        // Create immutable buffer (lives on GPU).
        m_memoryIndex = m_base->CreateBuffer(size, vk::BufferUsageFlagBits::eIndexBuffer | vk::BufferUsageFlagBits::eTransferDst, vma::MemoryUsage::eGpuOnly, &m_immutableBuffer);

        // Copy staging into immutable buffer.
        Commands::SubmitCopyCommandImmediate(*m_base, stagingBuffer, m_immutableBuffer, size);

        // Destroy the staging buffer.
        m_base->DestroyBuffer(stagingBufferMemoryIndex);
    }

    void ImmutableBuffer::Bind(nc::graphics::Graphics* graphics, const std::vector<Vertex>& data)
    {
        m_base = graphics->GetBasePtr();

        auto size = static_cast<uint32_t>(sizeof(Vertex) * data.size());

        // Create staging buffer (lives on CPU).
        vk::Buffer stagingBuffer;
        auto stagingBufferMemoryIndex = m_base->CreateBuffer(size, vk::BufferUsageFlagBits::eTransferSrc, vma::MemoryUsage::eCpuOnly, &stagingBuffer);

        // Map the data onto the staging buffer.
        void* mappedData;
        auto* allocator = m_base->GetAllocator();
        auto* allocation = m_base->GetBufferAllocation(stagingBufferMemoryIndex);

        allocator->mapMemory(*allocation, &mappedData);
        memcpy(mappedData, data.data(), size);
        allocator->unmapMemory(*allocation);

        // Create immutable buffer (lives on GPU).
        m_memoryIndex = m_base->CreateBuffer(size, vk::BufferUsageFlagBits::eVertexBuffer | vk::BufferUsageFlagBits::eTransferDst, vma::MemoryUsage::eGpuOnly, &m_immutableBuffer);

        // Copy staging into immutable buffer.
        Commands::SubmitCopyCommandImmediate(*m_base, stagingBuffer, m_immutableBuffer, size);

        // Destroy the staging buffer.
        m_base->DestroyBuffer(stagingBufferMemoryIndex);
    }

    void ImmutableBuffer::Bind(nc::graphics::Graphics* graphics, const SceneData& data)
    {
        m_base = graphics->GetBasePtr();

        auto size = static_cast<uint32_t>(sizeof(SceneData));

        // Create staging buffer (lives on CPU).
        vk::Buffer stagingBuffer;
        auto stagingBufferMemoryIndex = m_base->CreateBuffer(size, vk::BufferUsageFlagBits::eTransferSrc, vma::MemoryUsage::eCpuOnly, &stagingBuffer);

        // Map the data onto the staging buffer.
        void* mappedData;
        auto* allocator = m_base->GetAllocator();
        auto* allocation = m_base->GetBufferAllocation(stagingBufferMemoryIndex);

        allocator->mapMemory(*allocation, &mappedData);
        memcpy(mappedData, &data, size);
        allocator->unmapMemory(*allocation);

        // Create immutable buffer (lives on GPU).
        m_memoryIndex = m_base->CreateBuffer(size, vk::BufferUsageFlagBits::eUniformBuffer | vk::BufferUsageFlagBits::eTransferDst, vma::MemoryUsage::eGpuOnly, &m_immutableBuffer);

        // Copy staging into immutable buffer.
        Commands::SubmitCopyCommandImmediate(*m_base, stagingBuffer, m_immutableBuffer, size);

        // Destroy the staging buffer.
        m_base->DestroyBuffer(stagingBufferMemoryIndex);
    }

    vk::Buffer* ImmutableBuffer::GetBuffer()
    {
        return &m_immutableBuffer;
    }

    void ImmutableBuffer::Clear()
    {
        if (m_immutableBuffer)
        {
            m_base->DestroyBuffer(m_memoryIndex);
            m_immutableBuffer = nullptr;
        }

        m_base = nullptr;
    }
}