#pragma once

#include "graphics/api/vulkan/GpuAllocator.h"

#include "ncasset/AssetsFwd.h"

#include <span>

namespace nc::graphics
{

// Buffer that is intended for infrequent or one-time writes on the CPU, and frequent reads on the GPU.
class ImmutableBuffer
{
    public:
        ImmutableBuffer();
        ImmutableBuffer(GpuAllocator* allocator, std::span<const uint32_t> data);
        ImmutableBuffer(GpuAllocator* allocator, std::span<const asset::MeshVertex> data);
        ImmutableBuffer(ImmutableBuffer&&) = default;
        ImmutableBuffer& operator=(ImmutableBuffer&&) = default;
        ImmutableBuffer& operator=(const ImmutableBuffer&) = delete;
        ImmutableBuffer(const ImmutableBuffer&) = delete;

        auto GetBuffer() const noexcept -> vk::Buffer { return m_buffer; }
        void Clear() noexcept;

    private:
        GpuAllocation<vk::Buffer> m_buffer;
};
}