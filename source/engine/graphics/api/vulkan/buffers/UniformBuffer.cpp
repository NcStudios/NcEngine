#include "UniformBuffer.h"

namespace nc::graphics::vulkan
{
UniformBuffer::UniformBuffer(GpuAllocator* allocator, const void*, uint32_t size)
    : m_allocator{allocator},
      m_alignedSize{m_allocator->PadBufferOffsetAlignment(size, vk::DescriptorType::eUniformBuffer)},
      m_buffer{m_allocator->CreateBuffer(m_alignedSize, vk::BufferUsageFlagBits::eUniformBuffer, vma::MemoryUsage::eCpuToGpu)},
      m_info{vk::DescriptorBufferInfo{m_buffer, 0, m_alignedSize}}
{}

UniformBuffer::UniformBuffer(UniformBuffer&& other) noexcept
    : m_allocator{std::exchange(other.m_allocator, nullptr)},
      m_alignedSize{std::exchange(other.m_alignedSize, 0u)},
      m_buffer{std::exchange(other.m_buffer, GpuAllocation<vk::Buffer>{})},
      m_info{std::exchange(other.m_info, vk::DescriptorBufferInfo{})}
{}

UniformBuffer& UniformBuffer::operator=(UniformBuffer&& other) noexcept
{
    m_allocator = std::exchange(other.m_allocator, nullptr);
    m_alignedSize = std::exchange(other.m_alignedSize, 0u);
    m_buffer = std::exchange(other.m_buffer, GpuAllocation<vk::Buffer>{});
    m_info = std::exchange(other.m_info, vk::DescriptorBufferInfo{});
    return *this;
}

void UniformBuffer::Clear()
{
    void* mappedData = m_allocator->Map(m_buffer.Allocation());
    memset(mappedData, 0u, m_alignedSize);
    m_allocator->Unmap(m_buffer.Allocation());
}

void UniformBuffer::Bind(const void* data, uint32_t size)
{
    void* mappedData = m_allocator->Map(m_buffer.Allocation());
    memcpy(mappedData, data, size);
    m_allocator->Unmap(m_buffer.Allocation());
}
} // namespace nc::graphics::vulkan
