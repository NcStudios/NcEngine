#include "ImmutableImage.h"

namespace nc::graphics
{
    ImmutableImage::ImmutableImage()
        : m_image{},
          m_view{}
    {
    }

    ImmutableImage::ImmutableImage(vk::Device device, GpuAllocator* allocator, const unsigned char* pixels, uint32_t width, uint32_t height, bool isNormal)
        : m_image{allocator->CreateTexture(pixels, width, height, isNormal)},
          m_view{CreateTextureView(device, m_image, isNormal)}
    {
    }

    void ImmutableImage::Clear() noexcept
    {
        m_image.Release();
        m_view.reset();
    }
}