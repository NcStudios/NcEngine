#include "ImmutableImage.h"
#include "graphics/Base.h"

typedef unsigned char stbi_uc;

namespace nc::graphics
{
    ImmutableImage::ImmutableImage()
        : m_image{},
          m_view{}
    {
    }

    ImmutableImage::ImmutableImage(Base* base, GpuAllocator* allocator, stbi_uc* pixels, uint32_t width, uint32_t height)
        : m_image{allocator->CreateTexture(pixels, width, height)},
          m_view{CreateTextureView(base->GetDevice(), m_image)}
    {
    }

    void ImmutableImage::Clear() noexcept
    {
        m_image.Release();
        m_view.reset();
    }
}