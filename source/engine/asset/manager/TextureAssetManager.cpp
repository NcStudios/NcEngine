#include "TextureAssetManager.h"
#include "AssetUtilities.h"
#include "asset/Assets.h"
#include "asset/AssetData.h"

#include "ncasset/Import.h"

namespace nc::asset
{
TextureAssetManager::TextureAssetManager(const std::string& texturesAssetDirectory, uint32_t maxTextures)
    : m_assetDirectory{texturesAssetDirectory},
      m_maxTextureCount{maxTextures},
      m_onUpdate{}
{
}

bool TextureAssetManager::Load(const std::string& path, bool isExternal, asset_flags_type flags)
{
    if (m_table.size() + 1 >= m_maxTextureCount)
    {
        throw NcError("Cannot exceed max texture count.");
    }

    if (IsLoaded(path))
    {
        return false;
    }

    const auto fullPath = isExternal ? path : m_assetDirectory + path;
    auto texture = asset::TextureWithId{asset::ImportTexture(fullPath), path, flags};
    m_table.emplace(path);
    m_onUpdate.Emit(asset::TextureUpdateEventData{
        asset::UpdateAction::Load,
        std::vector<std::string>{path},
        std::span<const asset::TextureWithId>{&texture, 1}
    });

    return true;
}

bool TextureAssetManager::Load(std::span<const std::string> paths, bool isExternal, asset_flags_type flags)
{
    if (m_table.size() + paths.size() >= m_maxTextureCount)
    {
        throw NcError("Cannot exceed max texture count.");
    }

    auto textures = std::vector<TextureWithId>{};

    auto idsToLoad = std::vector<std::string>{};
    idsToLoad.reserve(paths.size());

    for (const auto& path : paths)
    {
        if (IsLoaded(path))
        {
            continue;
        }

        m_table.emplace(path);
        const auto fullPath = isExternal ? path : m_assetDirectory + path;
        textures.emplace_back(ImportTexture(fullPath), path, flags);
        idsToLoad.push_back(path);
    }

    if (!idsToLoad.empty())
    {
        m_onUpdate.Emit(TextureUpdateEventData{
            UpdateAction::Load,
            std::move(idsToLoad),
            std::span<const TextureWithId>{textures}
        });
    }

    return true;
}

bool TextureAssetManager::Unload(const std::string& path, asset_flags_type)
{
    if (!m_table.erase(path))
        return false;

    m_onUpdate.Emit(TextureUpdateEventData{
        UpdateAction::Unload,
        std::vector<std::string>{path},
        std::span<const TextureWithId>{}
    });

    return true;
}

void TextureAssetManager::UnloadAll(asset_flags_type)
{
    m_table.clear();
    m_onUpdate.Emit(TextureUpdateEventData{
        UpdateAction::UnloadAll,
        {},
        {}
    });
    /** No need to send signal to GPU - no need to write an empty buffer to the GPU. **/
}

auto TextureAssetManager::Acquire(const std::string& path, asset_flags_type) const -> TextureView
{
    const auto hash = m_table.hash(path);
    const auto index = m_table.index(hash);
    NC_ASSERT(index != m_table.NullIndex, fmt::format("Asset is not loaded: '{}'", path));
    return TextureView
    {
        .id = hash,
        .index = static_cast<uint32_t>(index)
    };
}

bool TextureAssetManager::IsLoaded(const std::string& path, asset_flags_type) const
{
    return m_table.contains(path);
}

auto TextureAssetManager::OnUpdate() -> Signal<const TextureUpdateEventData&>&
{
    return m_onUpdate;
}

auto TextureAssetManager::GetAllLoaded() const -> std::vector<std::string_view>
{
    return GetPaths(m_table.keys(), [](const auto& data)
    {
        return std::string_view{data};
    });
}
} // namespace nc::asset
