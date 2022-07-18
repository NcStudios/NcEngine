#include "TextureAssetManager.h"
#include "graphics/Base.h"
#include "graphics/Graphics.h"
#include "graphics/Initializers.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"
#undef STB_IMAGE_IMPLEMENATION

#include <cassert>
#include <fstream>

namespace
{
auto ReadTexture(const std::string& path) -> nc::TextureData
{
    int32_t width, height, numChannels;
    stbi_uc* pixels = stbi_load(path.c_str(), &width, &height, &numChannels, STBI_rgb_alpha);

    if(!pixels)
        throw nc::NcError("Failed to load texture file: " + path);

    return nc::TextureData{.pixels = pixels, .width = width, .height = height};
}
}

namespace nc
{
TextureAssetManager::TextureAssetManager(graphics::Graphics* graphics, const std::string& texturesAssetDirectory, uint32_t maxTextures)
    : m_accessors{},
        m_assetDirectory{texturesAssetDirectory},
        m_maxTextureCount{maxTextures},
        m_onUpdate{}
{
}

TextureAssetManager::~TextureAssetManager() noexcept
{

}

bool TextureAssetManager::Load(const std::string& path, bool isExternal)
{
    const auto index = static_cast<uint32_t>(m_accessors.size());

    if (index + 1 >= m_maxTextureCount)
        throw NcError("Cannot exceed max texture count.");

    if(IsLoaded(path))
        return false;

    m_accessors.emplace(path, index);
    const auto fullPath = isExternal ? path : m_assetDirectory + path;

    m_onUpdate.Emit
    (
        TextureBufferData
        {
            .ids = std::vector<std::string>{fullPath},
            .data = std::vector<TextureData>{ReadTexture(fullPath)},
            .updateAction = UpdateAction::Load
        }
    );
    return true;
}

bool TextureAssetManager::Load(std::span<const std::string> paths, bool isExternal)
{
    const auto newTextureCount = static_cast<uint32_t>(paths.size());
    auto nextTextureIndex = static_cast<uint32_t>(m_accessors.size());
    if (newTextureCount + nextTextureIndex >= m_maxTextureCount)
        throw NcError("Cannot exceed max texture count.");

    auto textureBufferData = TextureBufferData
    {
        .ids = std::vector<std::string>{},
        .data = std::vector<TextureData>{},
        .updateAction = UpdateAction::Load
    };

    for (const auto& path : paths)
    {
        if(IsLoaded(path))
            continue;

        m_accessors.emplace(path, nextTextureIndex++);
        const auto fullPath = isExternal ? path : m_assetDirectory + path;
        textureBufferData.data.push_back(std::move(ReadTexture(fullPath)));
        textureBufferData.ids.push_back(fullPath);
    }
    m_onUpdate.Emit(textureBufferData);
    return true;
}

bool TextureAssetManager::Unload(const std::string& path)
{
    auto removed = static_cast<bool>(m_accessors.erase(path));
    if(!removed)
        return false;
    
    m_onUpdate.Emit
    (
        TextureBufferData
        {
            .ids = std::vector<std::string>{path},
            .data = std::vector<TextureData>{},
            .updateAction = UpdateAction::Unload
        }
    );
    return true;
}

void TextureAssetManager::UnloadAll()
{
    m_accessors.clear();
    m_onUpdate.Emit
    (
        TextureBufferData
        {
            .ids = std::vector<std::string>{},
            .data = std::vector<TextureData>{},
            .updateAction = UpdateAction::UnloadAll
        }
    );
}

auto TextureAssetManager::Acquire(const std::string& path) const -> TextureView
{
    const auto it = m_accessors.find(path);
    if(it == m_accessors.cend())
        throw NcError("Asset is not loaded: " + path);
    
    return it->second;
}

bool TextureAssetManager::IsLoaded(const std::string& path) const
{
    return m_accessors.contains(path);
}
}