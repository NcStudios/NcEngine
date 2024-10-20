#pragma once

#include "graphics/api/vulkan/GpuAllocator.h"

#include "ncasset/AssetsFwd.h"

#include <span>

namespace nc::graphics::vulkan
{
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

struct MeshArrayBuffer
{
    MeshBuffer vertices;
    MeshBuffer indices;
};
} // namespace nc::graphics::vulkan
