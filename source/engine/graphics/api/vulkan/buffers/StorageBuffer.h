#pragma once

#include "graphics/api/vulkan/GpuAllocator.h"

namespace nc::graphics::vulkan
{
class StorageBuffer
{
    public:
        StorageBuffer();
        StorageBuffer(GpuAllocator* allocator, uint32_t size);
        StorageBuffer(StorageBuffer&&) noexcept;
        StorageBuffer& operator=(StorageBuffer&&) noexcept;
        StorageBuffer& operator=(const StorageBuffer&) = delete;
        StorageBuffer(const StorageBuffer&) = delete;

        vk::Buffer GetBuffer() const noexcept;
        void Clear() noexcept;
        void Map(const void* dataToMap, uint32_t elementSize, uint32_t elementCount);

    private:
        GpuAllocator* m_allocator;
        GpuAllocation<vk::Buffer> m_buffer;
        uint32_t m_memorySize;
};
} // namespace nc::graphics::vulkan
