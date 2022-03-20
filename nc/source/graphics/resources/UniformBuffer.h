#pragma once

#include "graphics/GpuAllocator.h"

namespace nc::graphics
{
    // Buffer that is intended for frequent writes on the CPU, and frequent reads on the GPU.
    class UniformBuffer
    {
        public:
            UniformBuffer();
            UniformBuffer(GpuAllocator* allocator, const void* data, uint32_t size);
            UniformBuffer(UniformBuffer&&);
            UniformBuffer& operator=(UniformBuffer&&);
            UniformBuffer& operator=(const UniformBuffer&) = delete;
            UniformBuffer(const UniformBuffer&) = delete;

            vk::Buffer GetBuffer();
            void Bind(const void* data, uint32_t size);
            void Clear();

        private:
            GpuAllocator* m_allocator;
            GpuAllocation<vk::Buffer> m_buffer;
    };
}