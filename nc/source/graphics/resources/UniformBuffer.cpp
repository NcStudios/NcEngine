#include "UniformBuffer.h"
#include "graphics/Graphics.h"

namespace nc::graphics
{
    UniformBuffer::UniformBuffer()
        : m_memoryIndex { 0 },
          m_uniformBuffer { nullptr }
    {
    }

    UniformBuffer::UniformBuffer(nc::graphics::Graphics* graphics, const EnvironmentData& data)
        : m_memoryIndex { 0 },
          m_uniformBuffer { nullptr }
    {
        m_base = graphics->GetBasePtr();
        auto size = static_cast<uint32_t>(sizeof(EnvironmentData));
        m_memoryIndex = m_base->CreateBuffer(size, vk::BufferUsageFlagBits::eUniformBuffer, vma::MemoryUsage::eCpuToGpu, &m_uniformBuffer);
        
        Bind(graphics, data);
    }

    UniformBuffer::~UniformBuffer() noexcept
    {
        if (m_uniformBuffer)
        {
            m_base->DestroyBuffer(m_memoryIndex);
            m_uniformBuffer = nullptr;
        }

        m_base = nullptr;
    }

    UniformBuffer::UniformBuffer(UniformBuffer&& other)
        : m_base{std::exchange(other.m_base, nullptr)},
          m_memoryIndex{std::exchange(other.m_memoryIndex, 0)},
          m_uniformBuffer{std::exchange(other.m_uniformBuffer, nullptr)}
    {
    }

    UniformBuffer& UniformBuffer::operator = (UniformBuffer&& other)
    {
        m_base = std::exchange(other.m_base, nullptr);
        m_memoryIndex = std::exchange(other.m_memoryIndex, 0);
        m_uniformBuffer = std::exchange(other.m_uniformBuffer, nullptr);
        return *this;
    }

    void UniformBuffer::Bind(nc::graphics::Graphics* graphics, const EnvironmentData& data)
    {
        m_base = graphics->GetBasePtr();

        auto size = static_cast<uint32_t>(sizeof(EnvironmentData));

        void* mappedData;
        auto* allocator = m_base->GetAllocator();
        auto* allocation = m_base->GetBufferAllocation(m_memoryIndex);

        allocator->mapMemory(*allocation, &mappedData);
        memcpy(mappedData, &data, size);
        allocator->unmapMemory(*allocation);
    }

    vk::Buffer* UniformBuffer::GetBuffer()
    {
        return &m_uniformBuffer;
    }

    void UniformBuffer::Clear()
    {
        if (m_uniformBuffer)
        {
            m_base->DestroyBuffer(m_memoryIndex);
            m_uniformBuffer = nullptr;
        }

        m_base = nullptr;
    }
}