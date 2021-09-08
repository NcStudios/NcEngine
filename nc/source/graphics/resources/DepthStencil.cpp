#include "DepthStencil.h"
#include "graphics/Base.h"

namespace nc::graphics
{
    DepthStencil::DepthStencil(Base* base, Vector2 dimensions)
    : m_base{base},
      m_image{},
      m_view{},
      m_memoryIndex{0}
    {
        auto depthFormat = base->GetDepthFormat();
        m_memoryIndex = m_base->CreateImage(depthFormat, dimensions, vk::ImageUsageFlagBits::eDepthStencilAttachment, &m_image);
        
        vk::ImageSubresourceRange imageSubresourceRange{};
        imageSubresourceRange.setBaseMipLevel(0);
        imageSubresourceRange.setLevelCount(1);
        imageSubresourceRange.setBaseArrayLayer(0);
        imageSubresourceRange.setLayerCount(1);
        imageSubresourceRange.setAspectMask(vk::ImageAspectFlagBits::eDepth);

        if (depthFormat >= vk::Format::eD16UnormS8Uint)
        {
            imageSubresourceRange.aspectMask |= vk::ImageAspectFlagBits::eStencil;
        }

        vk::ImageViewCreateInfo imageViewInfo{};
        imageViewInfo.setViewType(vk::ImageViewType::e2D);
        imageViewInfo.setImage(m_image);
        imageViewInfo.setFormat(depthFormat);
        imageViewInfo.setSubresourceRange(imageSubresourceRange);

        m_view = m_base->GetDevice().createImageView(imageViewInfo);
    }

    const vk::Image& DepthStencil::GetImage() const noexcept
    {
        return m_image;
    }

    const vk::ImageView& DepthStencil::GetImageView() const noexcept
    {
        return m_view;
    }

    DepthStencil::~DepthStencil()
    {
        m_base->GetDevice().destroyImageView(m_view);
        m_base->DestroyImage(m_memoryIndex);
    }
}