#include "ImmutableBuffer.h"

namespace
{
    template<class T>
    auto UsageFlags() -> vk::BufferUsageFlags;

    template<>
    auto UsageFlags<uint32_t>() -> vk::BufferUsageFlags
    {
        return vk::BufferUsageFlagBits::eIndexBuffer | vk::BufferUsageFlagBits::eTransferDst;
    }

    template<>
    auto UsageFlags<nc::Vertex>() -> vk::BufferUsageFlags
    {
        return vk::BufferUsageFlagBits::eVertexBuffer | vk::BufferUsageFlagBits::eTransferDst;
    }

    template<class T>
    auto CreateBuffer(nc::graphics::GpuAllocator* allocator, std::span<const T> data) -> nc::graphics::GpuAllocation<vk::Buffer>
    {
        const auto size = static_cast<uint32_t>(sizeof(T) * data.size());

        // Create staging buffer (lives on CPU).
        auto stagingBuffer = allocator->CreateBuffer(size, vk::BufferUsageFlagBits::eTransferSrc, vma::MemoryUsage::eCpuOnly);

        // Map the data onto the staging buffer.
        void* mappedData = allocator->Map(stagingBuffer.Allocation());
        memcpy(mappedData, data.data(), size);
        allocator->Unmap(stagingBuffer.Allocation());

        // Create immutable buffer (lives on GPU).
        auto buffer = allocator->CreateBuffer(size, UsageFlags<T>(), vma::MemoryUsage::eGpuOnly);

        // Copy staging into immutable buffer.
        allocator->CopyBuffer(stagingBuffer, buffer.Data(), size);

        // Destroy the staging buffer.
        stagingBuffer.Release();

        return buffer;
    }
}

namespace nc::graphics
{
    ImmutableBuffer::ImmutableBuffer()
        : m_buffer{}
    {
    }

    ImmutableBuffer::ImmutableBuffer(GpuAllocator* allocator, std::span<const uint32_t> data)
        : m_buffer{CreateBuffer(allocator, data)}
    {
    }

    ImmutableBuffer::ImmutableBuffer(GpuAllocator* allocator, std::span<const nc::Vertex> data)
        : m_buffer{CreateBuffer(allocator, data)}
    {
    }

    void ImmutableBuffer::Clear() noexcept
    {
        m_buffer.Release();
    }
}