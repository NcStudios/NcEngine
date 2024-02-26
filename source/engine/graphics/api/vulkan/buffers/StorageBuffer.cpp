#include "StorageBuffer.h"

#include "optick.h"

namespace nc::graphics::vulkan
{
StorageBuffer::StorageBuffer(GpuAllocator* allocator, uint32_t size)
    : m_allocator{allocator},
      m_buffer{},
      m_alignedSize{},
      m_info{},
      m_previousDataSize{0u}
{
    m_alignedSize = m_allocator->PadBufferOffsetAlignment(size, vk::DescriptorType::eStorageBuffer);
    m_buffer = m_allocator->CreateBuffer(m_alignedSize, vk::BufferUsageFlagBits::eStorageBuffer, vma::MemoryUsage::eCpuToGpu);
    m_info = vk::DescriptorBufferInfo{m_buffer, 0, m_alignedSize};
}

StorageBuffer::StorageBuffer(StorageBuffer&& other) noexcept
    : m_allocator{std::exchange(other.m_allocator, nullptr)},
      m_buffer{std::exchange(other.m_buffer, GpuAllocation<vk::Buffer>{})},
      m_alignedSize{other.m_alignedSize},
      m_info{vk::DescriptorBufferInfo{m_buffer, 0, m_alignedSize}}
{
}

StorageBuffer& StorageBuffer::operator=(StorageBuffer&& other) noexcept
{
    m_allocator = std::exchange(other.m_allocator, nullptr);
    m_buffer = std::exchange(other.m_buffer, GpuAllocation<vk::Buffer>{});
    m_alignedSize = other.m_alignedSize;
    m_info = vk::DescriptorBufferInfo{m_buffer, 0, m_alignedSize};
    std::exchange(other.m_info, vk::DescriptorBufferInfo{});
    return *this;
}

void StorageBuffer::Clear() noexcept
{
    OPTICK_CATEGORY("StorageBuffer::Clear", Optick::Category::Rendering);

    auto zeroBuffer = std::vector<char>(m_alignedSize);
    Bind(zeroBuffer.data(), m_alignedSize);
}

void StorageBuffer::Bind(const void* dataToMap, uint32_t dataSize)
{
    OPTICK_CATEGORY("StorageBuffer::Bind", Optick::Category::Rendering);
    
    auto allocation = m_buffer.Allocation();
    void* dataContainer = m_allocator->Map(allocation);

    if (m_previousDataSize > 0u)
    {
        auto zeroBuffer = std::vector<char>(m_previousDataSize);
        memcpy(dataContainer, zeroBuffer.data(), m_previousDataSize);
    }

    memcpy(dataContainer, dataToMap, dataSize);
    m_allocator->Unmap(allocation);
    m_previousDataSize = dataSize;
}
}