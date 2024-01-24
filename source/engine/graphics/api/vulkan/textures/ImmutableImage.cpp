#include "ImmutableImage.h"

namespace nc::graphics
{
    ImmutableImage::ImmutableImage()
        : m_mipLevels{},
          m_image{},
          m_view{}
    {
    }

    ImmutableImage::ImmutableImage(vk::Device device, GpuAllocator* allocator, const unsigned char* pixels, uint32_t width, uint32_t height, bool isNormal)
        : m_mipLevels{static_cast<uint32_t>(std::floor(std::log2(std::max(width, height)))) + 1},
          m_image{allocator->CreateTexture(pixels, width, height, m_mipLevels, isNormal)},
          m_view{CreateTextureView(device, m_image, isNormal, m_mipLevels)}
    {
    }

    void ImmutableImage::Clear() noexcept
    {
        m_image.Release();
        m_view.reset();
    }
}