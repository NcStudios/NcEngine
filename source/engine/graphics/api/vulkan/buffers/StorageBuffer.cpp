#include "StorageBuffer.h"

namespace nc::graphics::vulkan
{
StorageBuffer::StorageBuffer(GpuAllocator* allocator, uint32_t size)
    : m_allocator{allocator},
      m_buffer{},
      m_alignedSize{},
      m_info{}
{
    m_alignedSize = m_allocator->PadBufferOffsetAlignment(size, vk::DescriptorType::eStorageBuffer);
    m_buffer = m_allocator->CreateBuffer(m_alignedSize, vk::BufferUsageFlagBits::eStorageBuffer, vma::MemoryUsage::eCpuToGpu);
    m_info = vk::DescriptorBufferInfo{m_buffer, 0, m_alignedSize};
}

StorageBuffer::StorageBuffer(StorageBuffer&& other) noexcept
    : m_allocator{std::exchange(other.m_allocator, nullptr)},
      m_buffer{std::exchange(other.m_buffer, GpuAllocation<vk::Buffer>{})}
{
}

StorageBuffer& StorageBuffer::operator=(StorageBuffer&& other) noexcept
{
    m_allocator = std::exchange(other.m_allocator, nullptr);
    m_buffer = std::exchange(other.m_buffer, GpuAllocation<vk::Buffer>{});
    return *this;
}

void StorageBuffer::Clear() noexcept
{
    m_buffer.Release();
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