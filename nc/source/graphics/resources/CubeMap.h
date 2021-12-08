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
            CubeMap(nc::graphics::Graphics* graphics, const std::array<stbi_uc*, 6>& pixels, uint32_t width, uint32_t height, uint32_t cubeMapSize, const std::string& uid);
            ~CubeMap() noexcept;
            CubeMap(CubeMap&&) noexcept;
            CubeMap& operator=(CubeMap&&) noexcept;
            CubeMap& operator=(const CubeMap&) = delete;
            CubeMap(const CubeMap&) = delete;

            const vk::ImageView& GetImageView() const noexcept;
            void Bind(const std::array<stbi_uc*, 6>& pixels, uint32_t width, uint32_t height, uint32_t cubeMapSize);
            void Clear() noexcept;
            const std::string& GetUid() const noexcept;
            
        private:
            Base* m_base;
            uint32_t m_memoryIndex;
            vk::Image m_cubeMapImage;
            vk::UniqueImageView m_cubeMapview;
            std::string m_uid;
    };
}