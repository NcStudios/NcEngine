#pragma once

#include "graphics/api/vulkan/GpuAllocator.h"

namespace nc::graphics::vulkan
{
    class ImageArrayBuffer
    {
        public:
            ImageArrayBuffer();
            ImageArrayBuffer(vk::Device device, GpuAllocator* allocator, const unsigned char* pixels, uint32_t width, uint32_t height, bool isNormal);
            ImageArrayBuffer(ImageArrayBuffer&&) = default;
            ImageArrayBuffer& operator=(ImageArrayBuffer&&) = default;
            ImageArrayBuffer& operator=(const ImageArrayBuffer&) = delete;
            ImageArrayBuffer(const ImageArrayBuffer&) = delete;

            auto GetImageView() noexcept -> vk::ImageView& { return m_view.get(); }
            void Clear() noexcept;

        private:
            GpuAllocation<vk::Image> m_image;
            vk::UniqueImageView m_view;
    };
} // namespace nc::graphics::vulkan
