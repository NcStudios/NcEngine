#include "ImmutableImage.h"

namespace nc::graphics
{
    ImmutableImage::ImmutableImage()
        : m_image{},
          m_view{}
    {
    }

    ImmutableImage::ImmutableImage(vk::Device device, GpuAllocator* allocator, unsigned char* pixels, uint32_t width, uint32_t height)
        : m_image{allocator->CreateTexture(pixels, width, height)},
          m_view{CreateTextureView(device, m_image)}
    {
    }

    void ImmutableImage::Clear() noexcept
    {
        m_image.Release();
        m_view.reset();
    }
}