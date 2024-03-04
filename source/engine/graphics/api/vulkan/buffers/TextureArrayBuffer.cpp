#include "TextureArrayBuffer.h"
#include "graphics/api/vulkan/Initializers.h"

namespace nc::graphics::vulkan
{
Image::Image()
    : m_image{},
      m_view{}
{}

Image::Image(GpuAllocator* allocator, const unsigned char* pixels, uint32_t width, uint32_t height, bool isNormal)
    : m_image{allocator->CreateTexture(pixels, width, height, isNormal)},
      m_view{allocator->CreateTextureView(m_image, isNormal)}
{}

void Image::Clear() noexcept
{
    m_image.Release();
    m_view.reset();
}

TextureArrayBuffer::TextureArrayBuffer(vk::Device device)
    : sampler{CreateTextureSampler(device, vk::SamplerAddressMode::eRepeat)}
{}
} // namespace nc::graphics::vulkan
