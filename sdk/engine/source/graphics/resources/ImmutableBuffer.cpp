#include "ImmutableBuffer.h"
#include "graphics/Base.h"
#include "graphics/Commands.h"

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
    auto UsageFlags<nc::graphics::Vertex>() -> vk::BufferUsageFlags
    {
        return vk::BufferUsageFlagBits::eVertexBuffer | vk::BufferUsageFlagBits::eTransferDst;
    }

    template<class T>
    auto CreateBuffer(nc::graphics::Base* base, nc::graphics::GpuAllocator* allocator, const std::vector<T>& data) -> nc::graphics::GpuAllocation<vk::Buffer>
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
        nc::graphics::Commands::SubmitCopyCommandImmediate(*base, stagingBuffer, buffer.Data(), size);

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

    ImmutableBuffer::ImmutableBuffer(Base* base, GpuAllocator* allocator, const std::vector<uint32_t>& data)
        : m_buffer{CreateBuffer(base, allocator, data)}
    {
    }

    ImmutableBuffer::ImmutableBuffer(Base* base, GpuAllocator* allocator, const std::vector<nc::graphics::Vertex>& data)
        : m_buffer{CreateBuffer(base, allocator, data)}
    {
    }

    void ImmutableBuffer::Clear() noexcept
    {
        m_buffer.Release();
    }
}