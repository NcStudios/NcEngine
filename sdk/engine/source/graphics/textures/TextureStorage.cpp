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

void TextureStorage::LoadTextureBuffer(const TextureBufferData& textureBufferData)
{
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

void TextureStorage::UnloadTextureBuffer(const TextureBufferData& textureBufferData)
{
    const auto& id = textureBufferData.ids[0];
    auto pos = std::ranges::find_if(m_textureBuffers, [&id](const auto& texture)
    {
        return texture.uid == id;
    });

    assert(pos != m_textureBuffers.end());
    m_textureBuffers.erase(pos);
    graphics::ShaderResourceService<graphics::TextureImageInfo>::Get()->Update(m_textureImageInfos);
}

void TextureStorage::UnloadAllTextureBuffer()
{
    /** No need to write an empty buffer to the GPU. **/
    m_textureBuffers.clear();
    m_textureImageInfos.clear();
}

void TextureStorage::UpdateBuffer(const TextureBufferData& textureBufferData)
{
    switch (textureBufferData.updateAction)
    {
        case UpdateAction::Load:
        {
            LoadTextureBuffer(textureBufferData);
            break;
        }
        case UpdateAction::Unload:
        {
            UnloadTextureBuffer(textureBufferData);
            break;
        }
        case UpdateAction::UnloadAll:
        {
            UnloadAllTextureBuffer();
            break;
        }
    }
}
}