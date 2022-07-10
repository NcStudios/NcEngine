#pragma once

#include "graphics/GpuAllocator.h"
#include "graphics/Vertex.h"

#include <vector>

namespace nc::graphics
{
    class Base;

    // Buffer that is intended for infrequent or one-time writes on the CPU, and frequent reads on the GPU.
    class ImmutableBuffer
    {
        public:
            ImmutableBuffer();
            ImmutableBuffer(Base* base, GpuAllocator* allocator, std::span<const uint32_t> data);
            ImmutableBuffer(Base* base, GpuAllocator* allocator, std::span<const nc::Vertex> data);
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