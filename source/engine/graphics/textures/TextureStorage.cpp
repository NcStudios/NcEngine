#include "TextureStorage.h"
#include "graphics/GpuOptions.h"
#include "graphics/GpuAllocator.h"
#include "graphics/textures/TextureManager.h"
#include "graphics/vk/Initializers.h"

namespace nc::graphics
{
TextureStorage::TextureStorage(GpuOptions* base, GpuAllocator* allocator, const nc::GpuAccessorSignals& gpuAccessorSignals)
    : m_base{base},
      m_allocator{allocator},
      m_textureBuffers{},
      m_sampler{graphics::CreateTextureSampler(base->GetDevice(), vk::SamplerAddressMode::eRepeat)},
      m_onTextureUpdate{gpuAccessorSignals.onTextureUpdate->Connect(this, &TextureStorage::UpdateBuffer)}
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

void TextureStorage::LoadTextureBuffer(const TextureBufferData& textureBufferData)
{
    for (auto i = 0ull; i < textureBufferData.ids.size(); ++i)
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

void TextureStorage::UnloadTextureBuffer(const TextureBufferData& textureBufferData)
{
    const auto& id = textureBufferData.ids[0];
    auto pos = std::ranges::find_if(m_textureBuffers, [&id](const auto& texture)
    {
        return texture.uid == id;
    });

    assert(pos != m_textureBuffers.end());
    m_textureBuffers.erase(pos);
    graphics::ShaderResourceService<graphics::TextureBuffer>::Get()->Update(m_textureBuffers);
}

void TextureStorage::UnloadAllTextureBuffer()
{
    /** No need to write an empty buffer to the GPU. **/
    m_textureBuffers.clear();
}
} // namespace nc::graphics