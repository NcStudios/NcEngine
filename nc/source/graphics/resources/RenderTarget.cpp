#include "RenderTarget.h"
#include "graphics/Base.h"

namespace nc::graphics
{
    RenderTarget::RenderTarget(Base* base, Vector2 dimensions, bool isDepthStencil)
        : m_base{base},
          m_image{},
          m_view{},
          m_memoryIndex{0}
    {
        auto format = isDepthStencil ? base->GetDepthFormat() : vk::Format::eR8G8B8A8Srgb;
        auto imageUseFlags = isDepthStencil ? vk::ImageUsageFlagBits::eDepthStencilAttachment : vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eTransientAttachment;
        m_memoryIndex = m_base->CreateImage(format, dimensions, imageUseFlags, vk::ImageCreateFlags(), 1, &m_image, vk::SampleCountFlagBits::e1);
        
        vk::ImageSubresourceRange imageSubresourceRange{};
        imageSubresourceRange.setBaseMipLevel(0);
        imageSubresourceRange.setLevelCount(1);
        imageSubresourceRange.setBaseArrayLayer(0);
        imageSubresourceRange.setLayerCount(1);
        if (isDepthStencil)
        {
            imageSubresourceRange.setAspectMask(vk::ImageAspectFlagBits::eDepth);
        }
        else
        {
            imageSubresourceRange.setAspectMask(vk::ImageAspectFlagBits::eColor);
        }

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

    RenderTarget::RenderTarget(Base* base, Vector2 dimensions, vk::Format format, bool isDepthStencil)
    : m_base{base},
      m_image{},
      m_view{},
      m_memoryIndex{0}
    {
        auto imageUseFlags = isDepthStencil ? vk::ImageUsageFlagBits::eDepthStencilAttachment | vk::ImageUsageFlagBits::eSampled : vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eTransientAttachment;
        
        m_memoryIndex = m_base->CreateImage(format, dimensions, imageUseFlags, vk::ImageCreateFlags(), 1, &m_image, vk::SampleCountFlagBits::e1);
        
        vk::ImageSubresourceRange imageSubresourceRange{};
        imageSubresourceRange.setBaseMipLevel(0);
        imageSubresourceRange.setLevelCount(1);
        imageSubresourceRange.setBaseArrayLayer(0);
        imageSubresourceRange.setLayerCount(1);
        if (isDepthStencil)
        {
            imageSubresourceRange.setAspectMask(vk::ImageAspectFlagBits::eDepth);
        }
        else
        {
            imageSubresourceRange.setAspectMask(vk::ImageAspectFlagBits::eColor);
        }

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