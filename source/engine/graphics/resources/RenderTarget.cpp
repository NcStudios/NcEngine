#include "RenderTarget.h"
#include "graphics/Base.h"

namespace nc::graphics
{
    RenderTarget::RenderTarget(Base* base, GpuAllocator* allocator, Vector2 dimensions, bool isDepthStencil, vk::SampleCountFlagBits numSamples)
        : m_base{base},
          m_allocator{allocator},
          m_image{},
          m_view{}
    {
        auto format = isDepthStencil ? base->GetDepthFormat() : vk::Format::eR8G8B8A8Srgb;

        constexpr auto depthStencilImageUsage = vk::ImageUsageFlagBits::eDepthStencilAttachment;
        constexpr auto colorImageUsage = vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eTransientAttachment;

        auto imageUseFlags = isDepthStencil ? depthStencilImageUsage : colorImageUsage;
        m_image = m_allocator->CreateImage(format, dimensions, imageUseFlags, vk::ImageCreateFlags(), 1, numSamples);

        auto aspectMask = isDepthStencil ? vk::ImageAspectFlagBits::eDepth : vk::ImageAspectFlagBits::eColor;

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

        m_view = m_base->GetDevice().createImageView(imageViewInfo);
    }

    RenderTarget::RenderTarget(Base* base, GpuAllocator* allocator, Vector2 dimensions, bool isDepthStencil, vk::SampleCountFlagBits numSamples, vk::Format format)
        : m_base{base},
          m_allocator{allocator},
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

        m_view = m_base->GetDevice().createImageView(imageViewInfo);
    }

    RenderTarget::~RenderTarget() noexcept
    {
        m_base->GetDevice().destroyImageView(m_view);
    }

    vk::Image RenderTarget::GetImage() const noexcept
    {
        return m_image;
    }

    const vk::ImageView& RenderTarget::GetImageView() const noexcept
    {
        return m_view;
    }
}