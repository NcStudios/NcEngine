#pragma once

#include "utility/Signal.h"
#include "graphics/shader_resource/ShaderResourceBus.h"
#include "graphics/shader_resource/TextureArrayBufferHandle.h"

#include "ncasset/Assets.h"

namespace nc
{
namespace asset
{
struct TextureUpdateEventData;
} // namespace asset

namespace graphics
{
class TextureSystem
{
    public:
        TextureSystem(ShaderResourceBus* shaderResourceBus, Signal<const asset::TextureUpdateEventData&>& onTextureArrayBufferUpdate, uint32_t maxTextures);
        void UpdateTextureArrayBuffer(const asset::TextureUpdateEventData& eventData);

    private:
        TextureArrayBufferHandle m_textureArrayBuffer;
        nc::Connection<const asset::TextureUpdateEventData&> m_onTextureArrayBufferUpdate;
};
} // namespace graphics
} // namespace nc