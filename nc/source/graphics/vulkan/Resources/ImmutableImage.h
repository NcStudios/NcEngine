#pragma once

#include "graphics/vulkan/Base.h"
#include "graphics/vulkan/Commands.h"
#include "graphics/Graphics2.h"

#include "vulkan/vk_mem_alloc.hpp"
#include "stb/stb_image.h"

namespace nc::graphics::vulkan
{
    class ImmutableImage
    {
        public:
            ImmutableImage();
            ImmutableImage(nc::graphics::Graphics2* graphics, stbi_uc* pixels, uint32_t width, uint32_t height);
            ImmutableImage(ImmutableImage&&);
            ImmutableImage& operator = (ImmutableImage&&);
            ImmutableImage& operator = (const ImmutableImage&) = delete;
            ImmutableImage(const ImmutableImage&) = delete;
            ~ImmutableImage();

            vk::ImageView& GetImageView() noexcept;
            void Bind(nc::graphics::Graphics2* graphics, stbi_uc* pixels, uint32_t width, uint32_t height);
            void Clear();
            
        private:
            vulkan::Base* m_base;
            uint32_t m_memoryIndex;
            vk::Image m_immutableImage;
            vk::UniqueImageView m_view;
    };
}