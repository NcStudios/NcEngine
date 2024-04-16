#include "StorageBuffer.h"

#include "optick.h"

namespace nc::graphics::vulkan
{
StorageBuffer::StorageBuffer(GpuAllocator* allocator, uint32_t size)
    : m_allocator{allocator},
      m_alignedSize{m_allocator->PadBufferOffsetAlignment(size, vk::DescriptorType::eStorageBuffer)},
      m_buffer{m_allocator->CreateBuffer(m_alignedSize, vk::BufferUsageFlagBits::eStorageBuffer, vma::MemoryUsage::eCpuToGpu)},
      m_info{vk::DescriptorBufferInfo{m_buffer, 0, m_alignedSize}},
      m_previousDataSize{0u}
{}

StorageBuffer::StorageBuffer(StorageBuffer&& other) noexcept
    : m_allocator{std::exchange(other.m_allocator, nullptr)},
      m_alignedSize{other.m_alignedSize},
      m_buffer{std::exchange(other.m_buffer, GpuAllocation<vk::Buffer>{})},
      m_info{vk::DescriptorBufferInfo{m_buffer, 0, m_alignedSize}},
      m_previousDataSize{std::exchange(other.m_previousDataSize, 0u)}
{
    std::exchange(other.m_info, vk::DescriptorBufferInfo{});
}

StorageBuffer& StorageBuffer::operator=(StorageBuffer&& other) noexcept
{
    m_allocator = std::exchange(other.m_allocator, nullptr);
    m_buffer = std::exchange(other.m_buffer, GpuAllocation<vk::Buffer>{});
    m_alignedSize = std::exchange(other.m_alignedSize, 0u);
    m_info = vk::DescriptorBufferInfo{m_buffer, 0, m_alignedSize};
    std::exchange(other.m_info, vk::DescriptorBufferInfo{});
    return *this;
}

void StorageBuffer::Clear() noexcept
{
    auto allocation = m_buffer.Allocation();
    void* dataContainer = m_allocator->Map(allocation);

    if (m_previousDataSize > 0u)
    {
        memset(dataContainer, 0u, m_alignedSize);
    }

    m_allocator->Unmap(allocation);
    m_previousDataSize = 0u;
}

void StorageBuffer::Bind(const void* dataToMap, uint32_t dataSize)
{
    auto allocation = m_buffer.Allocation();
    void* dataContainer = m_allocator->Map(allocation);

    if (m_previousDataSize > dataSize)
    {
        const auto tailStart = reinterpret_cast<char*>(dataContainer) + dataSize;
        const auto tailLen = m_previousDataSize - dataSize;
        memset(tailStart, 0u, tailLen);
    }

    memcpy(dataContainer, dataToMap, dataSize);
    m_allocator->Unmap(allocation);
    m_previousDataSize = dataSize;
}
} // namespace nc::graphics::vulkan
