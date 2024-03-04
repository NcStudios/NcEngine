#include "TextureArrayBuffer.h"
#include "graphics/api/vulkan/Initializers.h"

namespace
{
auto GetMipLevels(uint32_t width, uint32_t height) -> uint32_t
{
    return static_cast<uint32_t>(std::floor(std::log2(std::max(width, height)))) + 1;
}
}
namespace nc::graphics::vulkan
{
Image::Image()
    : m_image{},
      m_view{}
{}

Image::Image(GpuAllocator* allocator, const unsigned char* pixels, uint32_t width, uint32_t height, bool isNormal)
    : m_image{allocator->CreateTexture(pixels, width, height, GetMipLevels(width, height), isNormal)},
      m_view{allocator->CreateTextureView(m_image, GetMipLevels(width, height), isNormal)}
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
