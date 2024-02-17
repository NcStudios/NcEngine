#pragma once

#include "graphics/api/vulkan/GpuAllocator.h"

namespace nc::graphics::vulkan
{
// Buffer that is intended for frequent writes on the CPU, and frequent reads on the GPU.
class UniformBuffer
{
    public:
        UniformBuffer(GpuAllocator* allocator, const void* data, uint32_t size);
        UniformBuffer(UniformBuffer&&) noexcept;
        UniformBuffer& operator=(UniformBuffer&&) noexcept;
        UniformBuffer& operator=(const UniformBuffer&) = delete;
        UniformBuffer(const UniformBuffer&) = delete;

        void Bind(const void* data, uint32_t size);
        void Clear();
        auto GetInfo() noexcept -> vk::DescriptorBufferInfo* {return &m_info;}

    private:
        GpuAllocator* m_allocator;
        GpuAllocation<vk::Buffer> m_buffer;
        uint32_t m_alignedSize;
        vk::DescriptorBufferInfo m_info;
};
} // namespace nc::graphics::vulkan
