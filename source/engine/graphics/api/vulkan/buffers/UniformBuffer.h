#pragma once

#include "graphics/api/vulkan/GpuAllocator.h"

namespace nc::graphics::vulkan
{
    // Buffer that is intended for frequent writes on the CPU, and frequent reads on the GPU.
    class UniformBuffer
    {
        public:
            UniformBuffer();
            UniformBuffer(GpuAllocator* allocator, const void* data, uint32_t size);
            UniformBuffer(UniformBuffer&&) noexcept;
            UniformBuffer& operator=(UniformBuffer&&) noexcept;
            UniformBuffer& operator=(const UniformBuffer&) = delete;
            UniformBuffer(const UniformBuffer&) = delete;

            vk::Buffer GetBuffer();
            void Bind(const void* data, uint32_t size);
            void Clear();

        private:
            GpuAllocator* m_allocator;
            GpuAllocation<vk::Buffer> m_buffer;
    };
} // namespace nc::graphics::vulkan
