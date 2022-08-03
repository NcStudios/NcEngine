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
      m_textureImageInfos{},
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

    m_textureBuffers.resize(0);
}

void TextureStorage::UpdateBuffer(const TextureBufferData& textureBufferData)
{
    m_textureBuffers.clear();
    m_textureBuffers.resize(0);

    m_textureImageInfos.clear();
    m_textureImageInfos.resize(0);

    for (auto i = 0; i < textureBufferData.data.size(); ++i)
    {
        auto& textureData = textureBufferData.data[i];
        auto& textureId = textureBufferData.ids[i];

        m_textureBuffers.emplace_back
        (
            nc::graphics::ImmutableImage(m_base, m_allocator, textureData.pixels, textureData.width, textureData.height),
            textureId
        );

        m_textureImageInfos.emplace_back(nc::graphics::CreateDescriptorImageInfo(m_sampler.get(), m_textureBuffers[i].image.GetImageView(), vk::ImageLayout::eShaderReadOnlyOptimal));
    }

    graphics::ShaderResourceService<graphics::TextureImageInfo>::Get()->Update(m_textureImageInfos);
}
}