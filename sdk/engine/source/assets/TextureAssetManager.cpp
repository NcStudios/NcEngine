#include "TextureAssetManager.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"
#undef STB_IMAGE_IMPLEMENATION

#include <cassert>
#include <fstream>

namespace
{
auto ReadTexture(const std::string& path, const std::string& uid) -> nc::TextureData
{
    int32_t width, height, numChannels;
    stbi_uc* pixels = stbi_load(path.c_str(), &width, &height, &numChannels, STBI_rgb_alpha);

    if(!pixels)
        throw nc::NcError("Failed to load texture file: " + path);

    return nc::TextureData{pixels, width, height, uid};
}
} // anonymous namespace

namespace nc
{
TextureAssetManager::TextureAssetManager(const std::string& texturesAssetDirectory, uint32_t maxTextures)
    : m_textureData{},
      m_accessors{},
      m_assetDirectory{texturesAssetDirectory},
      m_maxTextureCount{maxTextures},
      m_onUpdate{}
{
    m_textureData.reserve(m_maxTextureCount);
    m_accessors.reserve(m_maxTextureCount);
}

bool TextureAssetManager::Load(const std::string& path, bool isExternal)
{
    const auto index = static_cast<uint32_t>(m_accessors.size());

    if (index + 1 >= m_maxTextureCount)
        throw NcError("Cannot exceed max texture count.");

    if(IsLoaded(path))
        return false;

    m_accessors.emplace_back(index);
    const auto fullPath = isExternal ? path : m_assetDirectory + path;
    m_textureData.push_back(ReadTexture(fullPath, path));

    m_onUpdate.Emit
    (
        TextureBufferData(UpdateAction::Load, std::vector<std::string>{path}, std::span<const TextureData>{&m_textureData.back(), 1})
    );
    return true;
}

bool TextureAssetManager::Load(std::span<const std::string> paths, bool isExternal)
{
    const auto newItemsIndex = m_accessors.size();
    const auto newTextureCount = static_cast<uint32_t>(paths.size());
    auto nextTextureIndex = static_cast<uint32_t>(newItemsIndex);
    if (newTextureCount + nextTextureIndex >= m_maxTextureCount)
        throw NcError("Cannot exceed max texture count.");

    auto idsToLoad = std::vector<std::string>{};
    idsToLoad.reserve(paths.size());

    for (const auto& path : paths)
    {
        if(IsLoaded(path))
        {
            continue;
        }
        
        m_accessors.emplace_back(nextTextureIndex++);
        const auto fullPath = isExternal ? path : m_assetDirectory + path;
        m_textureData.push_back(ReadTexture(fullPath, path));
        idsToLoad.push_back(path);
    }

    if (!idsToLoad.empty())
    {
        m_onUpdate.Emit
        (
            TextureBufferData(UpdateAction::Load, std::move(idsToLoad), std::span<const TextureData>{m_textureData.begin() + newItemsIndex, m_textureData.end()})
        );
    }
    return true;
}

bool TextureAssetManager::Unload(const std::string& path)
{
    auto pos = std::ranges::find_if(m_textureData, [&path](const auto& data)
    {
        return data.id == path;
    });

    if (pos == m_textureData.end())
    {
        return false;
    }

    auto index = std::distance(m_textureData.begin(), pos);
    m_accessors.erase(m_accessors.begin()+index);

    for(auto& textureView : m_accessors)
    {
        if(textureView.index > index) --textureView.index;
    }

    m_textureData.erase(m_textureData.begin()+index);

    m_onUpdate.Emit
    (
        TextureBufferData(UpdateAction::Unload, std::vector<std::string>{path}, std::span<const TextureData>{})
    );
    return true;
}

void TextureAssetManager::UnloadAll()
{
    m_accessors.clear();
    m_textureData.clear();
    /** No need to send signal to GPU - no need to write an empty buffer to the GPU. **/
}

auto TextureAssetManager::Acquire(const std::string& path) const -> TextureView
{
    auto pos = std::ranges::find_if(m_textureData, [&path](const auto& data)
    {
        return data.id == path;
    });

    if(pos == m_textureData.end()) throw NcError("Asset is not loaded: " + path);

    auto index = std::distance(m_textureData.begin(), pos);
    return m_accessors[index];
}

bool TextureAssetManager::IsLoaded(const std::string& path) const
{
    auto pos = std::ranges::find_if(m_textureData, [&path](const auto& data)
    {
        return data.id == path;
    });

    return (pos != m_textureData.end());
}

auto TextureAssetManager::OnUpdate() -> Signal<const TextureBufferData&>*
{
    return &m_onUpdate;
}
} // namespace nc