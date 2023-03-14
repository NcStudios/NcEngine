#include "TextureStorage.h"
#include "TextureBuffer.h"
#include "asset/AssetData.h"
#include "graphics/api/vulkan/GpuAllocator.h"
#include "graphics/api/vulkan/Initializers.h"
#include "graphics/shader_resource/ShaderResourceService.h"

namespace nc::graphics
{
TextureStorage::TextureStorage(vk::Device device, GpuAllocator* allocator, Signal<const TextureUpdateEventData&>& onTextureUpdate)
    : m_device{device},
      m_allocator{allocator},
      m_textureBuffers{},
      m_sampler{graphics::CreateTextureSampler(device, vk::SamplerAddressMode::eRepeat)},
      m_onTextureUpdate{onTextureUpdate.Connect(this, &TextureStorage::UpdateBuffer)}
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

void TextureStorage::UpdateBuffer(const TextureUpdateEventData& eventData)
{
    switch (eventData.updateAction)
    {
        case UpdateAction::Load:
        {
            LoadTextureBuffer(eventData);
            break;
        }
        case UpdateAction::Unload:
        {
            UnloadTextureBuffer(eventData);
            break;
        }
        case UpdateAction::UnloadAll:
        {
            UnloadAllTextureBuffer();
            break;
        }
    }
}

void TextureStorage::LoadTextureBuffer(const TextureUpdateEventData& eventData)
{
    for (auto i = 0ull; i < eventData.ids.size(); ++i)
    {
        auto& textureData = eventData.data[i];
        auto& texture = textureData.texture;

        TextureBuffer textureBuffer
        {
            .image = graphics::ImmutableImage(m_device, m_allocator, texture.pixelData.data(), texture.width, texture.height),
            .imageInfo = graphics::CreateDescriptorImageInfo(m_sampler.get(), textureBuffer.image.GetImageView(), vk::ImageLayout::eShaderReadOnlyOptimal),
            .uid = textureData.id
        };

        m_textureBuffers.push_back(std::move(textureBuffer));
    }

    graphics::ShaderResourceService<graphics::TextureBuffer>::Get()->Update(m_textureBuffers);
}

void TextureStorage::UnloadTextureBuffer(const TextureUpdateEventData& eventData)
{
    const auto& id = eventData.ids[0];
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