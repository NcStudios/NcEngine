#pragma once

#include "graphics/GpuAllocator.h"
#include "ncmath/Vector.h"

namespace nc::graphics
{
    class RenderTarget
    {
        public:
            RenderTarget(vk::Device device, GpuAllocator* allocator, Vector2 dimensions, bool isDepthStencil, vk::SampleCountFlagBits numSamples, vk::Format depthFormat);

            vk::Image GetImage() const noexcept;
            const vk::ImageView& GetImageView() const noexcept;

        private:
            GpuAllocator* m_allocator;
            GpuAllocation<vk::Image> m_image;
            vk::UniqueImageView m_view;
    };
}