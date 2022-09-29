#include "RenderTarget.h"

namespace nc::graphics
{
    RenderTarget::RenderTarget(vk::Device device, GpuAllocator* allocator, Vector2 dimensions, bool isDepthStencil, vk::SampleCountFlagBits numSamples, vk::Format format)
        : m_allocator{allocator},
          m_image{},
          m_view{}
    {
        constexpr auto depthStencilImageUsage = vk::ImageUsageFlagBits::eDepthStencilAttachment | vk::ImageUsageFlagBits::eSampled;
        constexpr auto colorImageUsage = vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eTransientAttachment;
        const auto imageUseFlags = isDepthStencil ? depthStencilImageUsage : colorImageUsage;
        m_image = m_allocator->CreateImage(format, dimensions, imageUseFlags, vk::ImageCreateFlags(), 1, numSamples);
        const auto aspectMask = isDepthStencil ? vk::ImageAspectFlagBits::eDepth : vk::ImageAspectFlagBits::eColor;

        vk::ImageSubresourceRange imageSubresourceRange{};
        imageSubresourceRange.setBaseMipLevel(0);
        imageSubresourceRange.setLevelCount(1);
        imageSubresourceRange.setBaseArrayLayer(0);
        imageSubresourceRange.setLayerCount(1);
        imageSubresourceRange.setAspectMask(aspectMask);

        if (format >= vk::Format::eD16UnormS8Uint && isDepthStencil)
        {
            imageSubresourceRange.aspectMask |= vk::ImageAspectFlagBits::eStencil;
        }

        vk::ImageViewCreateInfo imageViewInfo{};
        imageViewInfo.setViewType(vk::ImageViewType::e2D);
        imageViewInfo.setImage(m_image);
        imageViewInfo.setFormat(format);
        imageViewInfo.setSubresourceRange(imageSubresourceRange);

        m_view = device.createImageViewUnique(imageViewInfo);
    }

    vk::Image RenderTarget::GetImage() const noexcept
    {
        return m_image;
    }

    const vk::ImageView& RenderTarget::GetImageView() const noexcept
    {
        return m_view.get();
    }
}