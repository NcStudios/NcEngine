#include "UniformBuffer.h"

namespace nc::graphics
{
    UniformBuffer::UniformBuffer()
        : m_memoryIndex { 0 },
          m_uniformBuffer { nullptr }
    {
    }

    UniformBuffer::UniformBuffer(Graphics* graphics, const void* data, uint32_t size)
        : m_memoryIndex{ 0 },
          m_uniformBuffer{ nullptr }
    {
        m_base = graphics->GetBasePtr();
        auto paddedSize = m_base->PadBufferOffsetAlignment(size, vk::DescriptorType::eUniformBuffer);
        m_memoryIndex = m_base->CreateBuffer(paddedSize, vk::BufferUsageFlagBits::eUniformBuffer, vma::MemoryUsage::eCpuToGpu, &m_uniformBuffer);

        Bind(graphics, data, size);
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

    void UniformBuffer::Bind(Graphics* graphics, const void* data, uint32_t size)
    {
        m_base = graphics->GetBasePtr();

        auto paddedSize = m_base->PadBufferOffsetAlignment(size, vk::DescriptorType::eUniformBuffer);

        void* mappedData;
        auto* allocator = m_base->GetAllocator();
        auto* allocation = m_base->GetBufferAllocation(m_memoryIndex);

        allocator->mapMemory(*allocation, &mappedData);
        memcpy(mappedData, data, paddedSize);
        allocator->unmapMemory(*allocation);
    }
}