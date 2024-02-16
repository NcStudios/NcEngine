#include "StorageBuffer.h"

namespace nc::graphics::vulkan
{
StorageBuffer::StorageBuffer(GpuAllocator* allocator, uint32_t size)
    : m_allocator{allocator},
      m_buffer{},
      m_memorySize{0}
{
    size = m_allocator->PadBufferOffsetAlignment(size, vk::DescriptorType::eStorageBuffer);
    m_buffer = m_allocator->CreateBuffer(size, vk::BufferUsageFlagBits::eStorageBuffer, vma::MemoryUsage::eCpuToGpu);
}

StorageBuffer::StorageBuffer(StorageBuffer&& other) noexcept
    : m_allocator{std::exchange(other.m_allocator, nullptr)},
      m_buffer{std::exchange(other.m_buffer, GpuAllocation<vk::Buffer>{})},
      m_memorySize{std::exchange(other.m_memorySize, 0)}
{
}

StorageBuffer& StorageBuffer::operator=(StorageBuffer&& other) noexcept
{
    m_allocator = std::exchange(other.m_allocator, nullptr);
    m_buffer = std::exchange(other.m_buffer, GpuAllocation<vk::Buffer>{});
    m_memorySize = std::exchange(other.m_memorySize, 0);
    return *this;
}

vk::Buffer StorageBuffer::GetBuffer() const noexcept
{
    return m_buffer;
}

void StorageBuffer::Clear() noexcept
{
    m_buffer.Release();
    m_memorySize = 0;
    m_allocator = nullptr;
}

void StorageBuffer::Map(const void* dataToMap, uint32_t dataSize)
{
    auto allocation = m_buffer.Allocation();
    void* dataContainer = m_allocator->Map(allocation);
    memcpy(dataContainer, dataToMap, dataSize);
    m_allocator->Unmap(allocation);
}
}