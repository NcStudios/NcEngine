#pragma once

#include "graphics/GpuAllocator.h"
#include "math/Vector.h"

namespace nc::graphics
{
    class Base;

    class RenderTarget
    {
        public:
            RenderTarget(Base* base, GpuAllocator* allocator, Vector2 dimensions, bool isDepthStencil, vk::SampleCountFlagBits numSamples);
            RenderTarget(Base* base, GpuAllocator* allocator, Vector2 dimensions, bool isDepthStencil, vk::SampleCountFlagBits numSamples, vk::Format depthFormat);
            ~RenderTarget() noexcept;

            vk::Image GetImage() const noexcept;
            const vk::ImageView& GetImageView() const noexcept;

        private:
            Base* m_base;
            GpuAllocator* m_allocator;
            GpuAllocation<vk::Image> m_image;
            vk::ImageView m_view;
    };
}