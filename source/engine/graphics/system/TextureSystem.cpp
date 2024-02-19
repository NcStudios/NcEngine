#include "TextureSystem.h"

#include "asset/AssetData.h"

#include <ranges>

namespace nc::graphics
{
TextureSystem::TextureSystem(ShaderResourceBus* shaderResourceBus, Signal<const asset::TextureUpdateEventData&>& onTextureArrayBufferUpdate, uint32_t maxTextures)
    : m_textureArray{},
      m_textureArrayBuffer{shaderResourceBus->CreateTextureArrayBuffer(maxTextures, ShaderStage::Fragment, 2, 1, true)},
      m_onTextureArrayBufferUpdate{onTextureArrayBufferUpdate.Connect(this, &TextureSystem::UpdateTextureArrayBuffer)}{}

void TextureSystem::UpdateTextureArrayBuffer(const asset::TextureUpdateEventData& eventData)
{
    switch (eventData.updateAction)
    {
        case asset::UpdateAction::Load:
        {
            auto incomingSize = m_textureArray.size() + eventData.data.size();
            m_textureArray.reserve(incomingSize);

            std::ranges::transform(eventData.data, std::back_inserter(m_textureArray),
            [](const asset::TextureWithId& texWithId) -> asset::TextureWithId { return texWithId; });
            m_textureArrayBuffer.Update(m_textureArray);
            break;
        }
        case asset::UpdateAction::Unload:
        {
            const auto& id = eventData.ids[0];
            auto pos = std::ranges::find_if(m_textureArray, [&id](const auto& texture)
            {
                return texture.id == id;
            });

            assert(pos != m_textureArray.end());
            m_textureArray.erase(pos);
            m_textureArrayBuffer.Update(m_textureArray);
            break;
        }
        case asset::UpdateAction::UnloadAll:
        {
            m_textureArray.clear();
            m_textureArrayBuffer.Clear();
            break;
        }
    }
}
} // namespace nc::graphics

