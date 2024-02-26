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
        GpuAllocation<vk::Buffer> m_buffer;
        uint32_t m_alignedSize;
        uint32_t m_previousDataSize;
        vk::DescriptorBufferInfo m_info;
};
} // namespace nc::graphics::vulkan
