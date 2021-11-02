#pragma once

#include "graphics/Base.h"
#include "graphics/Commands.h"
#include "graphics/Graphics.h"

#include "vulkan/vk_mem_alloc.hpp"
#include "stb/stb_image.h"

namespace nc::graphics
{
    class CubeMap
    {
        public:
            CubeMap();
            CubeMap(nc::graphics::Graphics* graphics, stbi_uc* pixels, uint32_t width, uint32_t height);
            ~CubeMap() noexcept;
            CubeMap(CubeMap&&) noexcept;
            CubeMap& operator=(CubeMap&&) noexcept;
            CubeMap& operator=(const CubeMap&) = delete;
            CubeMap(const CubeMap&) = delete;

            vk::ImageView& GetImageView() noexcept;
            void Bind(nc::graphics::Graphics* graphics, stbi_uc* pixels, uint32_t width, uint32_t height);
            void Clear() noexcept;
            
        private:
            Base* m_base;
            uint32_t m_memoryIndex;
            vk::Image m_immutableImage;
            vk::UniqueImageView m_view;
    };
}