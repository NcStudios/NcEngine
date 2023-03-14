#include "TextureAssetManager.h"
#include "asset/AssetData.h"

#include "ncasset/Import.h"

namespace nc
{
TextureAssetManager::TextureAssetManager(const std::string& texturesAssetDirectory, uint32_t maxTextures)
    : m_textureData{},
      m_assetDirectory{texturesAssetDirectory},
      m_maxTextureCount{maxTextures},
      m_onUpdate{}
{
    m_textureData.reserve(m_maxTextureCount);
}

bool TextureAssetManager::Load(const std::string& path, bool isExternal)
{
    const auto index = static_cast<uint32_t>(m_textureData.size());

    if (index + 1 >= m_maxTextureCount)
    {
        throw NcError("Cannot exceed max texture count.");
    }

    if (IsLoaded(path))
    {
        return false;
    }

    const auto fullPath = isExternal ? path : m_assetDirectory + path;
    m_textureData.emplace_back(asset::ImportTexture(fullPath), path);

    m_onUpdate.Emit(TextureBufferData{
        UpdateAction::Load,
        std::vector<std::string>{path},
        std::span<const TextureData>{&m_textureData.back(), 1}
    });

    return true;
}

bool TextureAssetManager::Load(std::span<const std::string> paths, bool isExternal)
{
    const auto newItemsIndex = m_textureData.size();
    const auto newTextureCount = static_cast<uint32_t>(paths.size());
    auto nextTextureIndex = static_cast<uint32_t>(newItemsIndex);
    if (newTextureCount + nextTextureIndex >= m_maxTextureCount)
    {
        throw NcError("Cannot exceed max texture count.");
    }

    auto idsToLoad = std::vector<std::string>{};
    idsToLoad.reserve(paths.size());

    for (const auto& path : paths)
    {
        if (IsLoaded(path))
        {
            continue;
        }

        const auto fullPath = isExternal ? path : m_assetDirectory + path;

        m_textureData.emplace_back(asset::ImportTexture(fullPath), path);
        idsToLoad.push_back(path);
    }

    if (!idsToLoad.empty())
    {
        m_onUpdate.Emit(TextureBufferData{
            UpdateAction::Load,
            std::move(idsToLoad),
            std::span<const TextureData>{m_textureData.begin() + newItemsIndex, m_textureData.end()}
        });
    }

    return true;
}

bool TextureAssetManager::Unload(const std::string& path)
{
    const auto pos = std::ranges::find_if(m_textureData, [&path](const auto& data)
    {
        return data.id == path;
    });

    if (pos == m_textureData.cend())
    {
        return false;
    }

    m_textureData.erase(pos);
    m_onUpdate.Emit(TextureBufferData{
        UpdateAction::Unload,
        std::vector<std::string>{path},
        std::span<const TextureData>{}
    });

    return true;
}

void TextureAssetManager::UnloadAll()
{
    m_textureData.clear();
    /** No need to send signal to GPU - no need to write an empty buffer to the GPU. **/
}

auto TextureAssetManager::Acquire(const std::string& path) const -> TextureView
{
    const auto pos = std::ranges::find_if(m_textureData, [&path](const auto& data)
    {
        return data.id == path;
    });

    if (pos == m_textureData.cend())
    {
        throw NcError("Asset is not loaded: " + path);
    }

    auto index = static_cast<uint32_t>(std::distance(m_textureData.cbegin(), pos));
    return TextureView{index};
}

bool TextureAssetManager::IsLoaded(const std::string& path) const
{
    auto pos = std::ranges::find_if(m_textureData, [&path](const auto& data)
    {
        return data.id == path;
    });

    return (pos != m_textureData.end());
}

auto TextureAssetManager::OnUpdate() -> Signal<const TextureBufferData&>&
{
    return m_onUpdate;
}
} // namespace nc