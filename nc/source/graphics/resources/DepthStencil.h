#pragma once

#include "vulkan/vk_mem_alloc.hpp"
#include "math/Vector.h"

namespace nc::graphics
{
    class Base; 

    class DepthStencil
    {
        public:
            DepthStencil(Base* base, Vector2 dimensions);
            DepthStencil(Base* base, Vector2 dimensions, vk::Format depthFormat);
            ~DepthStencil() noexcept;

            const vk::Image& GetImage() const noexcept;
            const vk::ImageView& GetImageView() const noexcept;
            const Vector2& GetDimensions() const noexcept;

        private:
            // External members
            Base* m_base;

            // Internal members
            Vector2 m_dimensions;
            vk::Image m_image;
            vk::ImageView m_view;
            uint32_t m_memoryIndex;
    };
}