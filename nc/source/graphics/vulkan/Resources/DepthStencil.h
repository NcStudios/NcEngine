#pragma once

#include "vulkan/vulkan.hpp"
#include "math/Vector.h"

namespace nc::graphics::vulkan
{
    class Base; 

    class DepthStencil
    {
        public:
            DepthStencil(vulkan::Base* base, Vector2 dimensions);
            ~DepthStencil();

            const vk::Image& GetImage() const noexcept;
            const vk::ImageView& GetImageView() const noexcept;

        private:
            // External members
            vulkan::Base* m_base;

            // Internal members
            vk::Image m_image;
            vk::ImageView m_view;
            uint32_t m_memoryIndex;
    };
}