#include "TextureStorage.h"
#include "graphics/Base.h"
#include "graphics/GpuAllocator.h"
#include "graphics/textures/TextureManager.h"
#include "graphics/vk/Initializers.h"

namespace nc::graphics
{
TextureStorage::TextureStorage(Base* base, GpuAllocator* allocator, const nc::GpuAccessorSignals& gpuAccessorSignals)
    : m_base{base},
      m_allocator{allocator},
      m_textureBuffers{},
      m_sampler{graphics::CreateTextureSampler(base->GetDevice(), vk::SamplerAddressMode::eRepeat)},
      m_onTextureUpdateConnection{gpuAccessorSignals.onTextureUpdate->Connect(this, &TextureStorage::UpdateBuffer)}
{
}

TextureStorage::~TextureStorage() noexcept
{
    for(auto& texture : m_textureBuffers)
    {
        texture.image.Clear();
    }

    m_textureBuffers.clear();
}

void TextureStorage::UpdateBuffer(const TextureBufferData& textureBufferData)
{
    m_textureBuffers.clear();

    for (auto i = 0; i < textureBufferData.data.size(); ++i)
    {
        auto& textureData = textureBufferData.data[i];

        TextureBuffer textureBuffer
        {
            .image = graphics::ImmutableImage(m_base, m_allocator, textureData.pixels.get(), textureData.width, textureData.height),
            .imageInfo = graphics::CreateDescriptorImageInfo(m_sampler.get(), textureBuffer.image.GetImageView(), vk::ImageLayout::eShaderReadOnlyOptimal),
            .uid = textureData.id
        };

        m_textureBuffers.push_back(std::move(textureBuffer));
    }

    graphics::ShaderResourceService<graphics::TextureBuffer>::Get()->Update(m_textureBuffers);
}
}