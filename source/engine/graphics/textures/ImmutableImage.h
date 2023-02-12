#pragma once

#include "graphics/GpuAllocator.h"

namespace nc::graphics
{
    class ImmutableImage
    {
        public:
            ImmutableImage();
            ImmutableImage(vk::Device device, GpuAllocator* allocator, const unsigned char* pixels, uint32_t width, uint32_t height);
            ImmutableImage(ImmutableImage&&) = default;
            ImmutableImage& operator=(ImmutableImage&&) = default;
            ImmutableImage& operator=(const ImmutableImage&) = delete;
            ImmutableImage(const ImmutableImage&) = delete;

            auto GetImageView() noexcept -> vk::ImageView& { return m_view.get(); }
            void Clear() noexcept;

        private:
            GpuAllocation<vk::Image> m_image;
            vk::UniqueImageView m_view;
    };
}