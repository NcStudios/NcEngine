#include "UniformBuffer.h"

namespace nc::graphics
{
    UniformBuffer::UniformBuffer()
        : m_allocator{nullptr},
          m_buffer{}
    {
    }

    UniformBuffer::UniformBuffer(GpuAllocator* allocator, const void* data, uint32_t size)
        : m_allocator{allocator},
          m_buffer{}
    {
        auto paddedSize = m_allocator->PadBufferOffsetAlignment(size, vk::DescriptorType::eUniformBuffer);
        m_buffer = m_allocator->CreateBuffer(paddedSize, vk::BufferUsageFlagBits::eUniformBuffer, vma::MemoryUsage::eCpuToGpu);
        Bind(data, size);
    }

    UniformBuffer::UniformBuffer(UniformBuffer&& other) noexcept
        : m_allocator{std::exchange(other.m_allocator, nullptr)},
          m_buffer{std::exchange(other.m_buffer, GpuAllocation<vk::Buffer>{})}
    {
    }

    UniformBuffer& UniformBuffer::operator=(UniformBuffer&& other) noexcept
    {
        m_allocator = std::exchange(other.m_allocator, nullptr);
        m_buffer = std::exchange(other.m_buffer, GpuAllocation<vk::Buffer>{});
        return *this;
    }

    vk::Buffer UniformBuffer::GetBuffer()
    {
        return m_buffer;
    }

    void UniformBuffer::Clear()
    {
        m_buffer.Release();
        m_allocator = nullptr;
    }

    void UniformBuffer::Bind(const void* data, uint32_t size)
    {
        auto paddedSize = m_allocator->PadBufferOffsetAlignment(size, vk::DescriptorType::eUniformBuffer);
        void* mappedData = m_allocator->Map(m_buffer.Allocation());
        memcpy(mappedData, data, paddedSize);
        m_allocator->Unmap(m_buffer.Allocation());
    }
}