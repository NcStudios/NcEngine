#include "TextureSystem.h"

#include "asset/AssetData.h"

#include <ranges>

namespace nc::graphics
{
TextureSystem::TextureSystem(ShaderResourceBus* shaderResourceBus, Signal<const asset::TextureUpdateEventData&>& onTextureArrayBufferUpdate, uint32_t maxTextures)
    : m_textureArrayBuffer{shaderResourceBus->CreateTextureArrayBuffer(maxTextures, ShaderStage::Fragment, 2, 1)},
      m_onTextureArrayBufferUpdate{onTextureArrayBufferUpdate.Connect(this, &TextureSystem::UpdateTextureArrayBuffer)}{}

void TextureSystem::UpdateTextureArrayBuffer(const asset::TextureUpdateEventData& eventData)
{
    switch (eventData.updateAction)
    {
        case asset::UpdateAction::Load:
        {
            m_textureArrayBuffer.Add(eventData.data);
            break;
        }
        case asset::UpdateAction::Unload:
        {
            m_textureArrayBuffer.Remove(eventData.data);
            break;
        }
        case asset::UpdateAction::UnloadAll:
        {
            m_textureArrayBuffer.Clear();
            break;
        }
    }
}
} // namespace nc::graphics
