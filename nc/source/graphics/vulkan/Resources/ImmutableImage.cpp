#include "ImmutableImage.h"

namespace nc::graphics::vulkan
{
    ImmutableImage::ImmutableImage()
    : m_memoryIndex { 0 },
      m_immutableImage { nullptr },
      m_view{}
    {
    }

    ImmutableImage::ImmutableImage(ImmutableImage&& other)
    : m_base{std::exchange(other.m_base, nullptr)},
      m_memoryIndex{std::exchange(other.m_memoryIndex, 0)},
      m_immutableImage{std::exchange(other.m_immutableImage, nullptr)},
      m_view{std::exchange(other.m_view, nullptr)}
    {
    }

    ImmutableImage& ImmutableImage::operator = (ImmutableImage&& other)
    {
        m_base= std::exchange(other.m_base, nullptr);
        m_memoryIndex = std::exchange(other.m_memoryIndex, 0);
        m_immutableImage = std::exchange(other.m_immutableImage, nullptr);
        m_view = std::exchange(other.m_view, nullptr);
        return *this;
    }

    ImmutableImage::~ImmutableImage()
    {
        if (m_view)
        {
            m_base->GetDevice().destroyImageView(m_view);
        }

        if (m_immutableImage)
        {
            m_base->DestroyImage(m_memoryIndex);
        }
    }

    void ImmutableImage::Bind(nc::graphics::Graphics2* graphics, stbi_uc* pixels, uint32_t width, uint32_t height)
    {
        m_base = graphics->GetBasePtr();
        m_memoryIndex = m_base->CreateTexture(pixels, width, height, &m_immutableImage);
        m_view = m_base->CreateTextureView(m_immutableImage);
    }

    vk::ImageView& ImmutableImage::GetImageView() noexcept
    {
        return m_view;
    }
}