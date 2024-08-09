#include "MeshArrayBuffer.h"

#include "ncasset/Assets.h"

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
auto UsageFlags<nc::asset::MeshVertex>() -> vk::BufferUsageFlags
{
    return vk::BufferUsageFlagBits::eVertexBuffer | vk::BufferUsageFlagBits::eTransferDst;
}

template<class T>
auto CreateBuffer(nc::graphics::vulkan::GpuAllocator* allocator, std::span<const T> data) -> nc::graphics::vulkan::GpuAllocation<vk::Buffer>
{
    const auto size = static_cast<uint32_t>(sizeof(T) * data.size());

    // Create staging buffer (lives on CPU).
    auto stagingBuffer = allocator->CreateBuffer(size, vk::BufferUsageFlagBits::eTransferSrc, VMA_MEMORY_USAGE_CPU_ONLY);

    // Map the data onto the staging buffer.
    void* mappedData = allocator->Map(stagingBuffer.Allocation());
    memcpy(mappedData, data.data(), size);
    allocator->Unmap(stagingBuffer.Allocation());

    // Create immutable buffer (lives on GPU).
    auto buffer = allocator->CreateBuffer(size, UsageFlags<T>(), VMA_MEMORY_USAGE_GPU_ONLY);

    // Copy staging into immutable buffer.
    allocator->CopyBuffer(stagingBuffer, buffer.Data(), size);

    // Destroy the staging buffer.
    stagingBuffer.Release();

    return buffer;
}
} // anonymous namespace

namespace nc::graphics::vulkan
{
MeshBuffer::MeshBuffer()
    : m_buffer{}
{
}

MeshBuffer::MeshBuffer(GpuAllocator* allocator, std::span<const uint32_t> data)
    : m_buffer{CreateBuffer(allocator, data)}
{
}

MeshBuffer::MeshBuffer(GpuAllocator* allocator, std::span<const asset::MeshVertex> data)
    : m_buffer{CreateBuffer(allocator, data)}
{
}

void MeshBuffer::Clear() noexcept
{
    m_buffer.Release();
}
} // namespace nc::graphics::vulkan
