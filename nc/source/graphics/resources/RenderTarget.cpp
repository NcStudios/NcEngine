#include "RenderTarget.h"
#include "graphics/Base.h"

#include <iostream>

namespace nc::graphics
{
    RenderTarget::RenderTarget(Base* base, Vector2 dimensions, bool isDepthStencil, vk::SampleCountFlagBits numSamples)
        : m_base{base},
          m_image{},
          m_view{},
          m_memoryIndex{0}
    {
        std::cout << "In RenderTarget ctor" << std::endl;
        auto format = isDepthStencil ? base->GetDepthFormat() : vk::Format::eR8G8B8A8Srgb;

        constexpr auto depthStencilImageUsage = vk::ImageUsageFlagBits::eDepthStencilAttachment;
        constexpr auto colorImageUsage = vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eTransientAttachment;

        auto imageUseFlags = isDepthStencil ? depthStencilImageUsage : colorImageUsage;
        m_memoryIndex = m_base->CreateImage(format, dimensions, imageUseFlags, vk::ImageCreateFlags(), 1, &m_image, numSamples);

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

    RenderTarget::RenderTarget(Base* base, Vector2 dimensions, vk::Format format, bool isDepthStencil, vk::SampleCountFlagBits numSamples)
    : m_base{base},
      m_image{},
      m_view{},
      m_memoryIndex{0}
    {
        constexpr auto depthStencilImageUsage = vk::ImageUsageFlagBits::eDepthStencilAttachment | vk::ImageUsageFlagBits::eSampled;
        constexpr auto colorImageUsage = vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eTransientAttachment;

        auto imageUseFlags = isDepthStencil ? depthStencilImageUsage : colorImageUsage;
        
        m_memoryIndex = m_base->CreateImage(format, dimensions, imageUseFlags, vk::ImageCreateFlags(), 1, &m_image, numSamples);
        
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

    RenderTarget::~RenderTarget() noexcept
    {
        m_base->GetDevice().destroyImageView(m_view);
        m_base->DestroyImage(m_memoryIndex);
    }

    const vk::Image& RenderTarget::GetImage() const noexcept
    {
        return m_image;
    }

    const vk::ImageView& RenderTarget::GetImageView() const noexcept
    {
        return m_view;
    }

    const Vector2& RenderTarget::GetDimensions() const noexcept
    {
        return m_dimensions;
    }
}