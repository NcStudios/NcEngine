#pragma once

#include "graphics/api/vulkan/GpuAllocator.h"

#include "ncasset/AssetsFwd.h"

#include <span>

namespace nc::graphics::vulkan
{
// Buffer that is intended for infrequent or one-time writes on the CPU, and frequent reads on the GPU.
class MeshBuffer
{
    public:
        MeshBuffer();
        MeshBuffer(GpuAllocator* allocator, std::span<const uint32_t> data);
        MeshBuffer(GpuAllocator* allocator, std::span<const asset::MeshVertex> data);
        MeshBuffer(MeshBuffer&&) = default;
        MeshBuffer& operator=(MeshBuffer&&) = default;
        MeshBuffer& operator=(const MeshBuffer&) = delete;
        MeshBuffer(const MeshBuffer&) = delete;

        auto GetBuffer() const noexcept -> vk::Buffer { return m_buffer; }
        void Clear() noexcept;

    private:
        GpuAllocation<vk::Buffer> m_buffer;
};
} // namespace nc::graphics::vulkan
