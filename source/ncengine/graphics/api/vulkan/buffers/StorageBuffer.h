#pragma once

#include "graphics/api/vulkan/GpuAllocator.h"

namespace nc::graphics::vulkan
{
class StorageBuffer
{
    public:
        StorageBuffer(GpuAllocator* allocator, uint32_t size);
        StorageBuffer(StorageBuffer&&) noexcept;
        StorageBuffer& operator=(StorageBuffer&&) noexcept;
        StorageBuffer& operator=(const StorageBuffer&) = delete;
        StorageBuffer(const StorageBuffer&) = delete;

        void Clear() noexcept;
        void Bind(const void* dataToMap, uint32_t dataSize);
        auto GetInfo() noexcept -> vk::DescriptorBufferInfo* {return &m_info;}

    private:
        GpuAllocator* m_allocator;
        uint32_t m_alignedSize;
        GpuAllocation<vk::Buffer> m_buffer;
        vk::DescriptorBufferInfo m_info;
        uint32_t m_previousDataSize;
};
} // namespace nc::graphics::vulkan
