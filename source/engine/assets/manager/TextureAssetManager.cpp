#include "TextureAssetManager.h"
#include "AssetUtilities.h"
#include "asset/Assets.h"
#include "asset/AssetData.h"

#include "ncasset/Import.h"

namespace nc
{
TextureAssetManager::TextureAssetManager(const std::string& texturesAssetDirectory, uint32_t maxTextures)
    : m_assetDirectory{texturesAssetDirectory},
      m_maxTextureCount{maxTextures},
      m_onUpdate{}
{
}

bool TextureAssetManager::Load(const std::string& path, bool isExternal, asset_flags_type flags)
{
    if (m_data.size() + 1 >= m_maxTextureCount)
    {
        throw NcError("Cannot exceed max texture count.");
    }

    if (IsLoaded(path))
    {
        return false;
    }

    const auto fullPath = isExternal ? path : m_assetDirectory + path;
    auto texture = asset::TextureWithId{asset::ImportTexture(fullPath), path, flags};
    m_data.emplace(path);
    m_onUpdate.Emit(asset::TextureUpdateEventData{
        asset::UpdateAction::Load,
        std::vector<std::string>{path},
        std::span<const asset::TextureWithId>{&texture, 1}
    });

    return true;
}

bool TextureAssetManager::Load(std::span<const std::string> paths, bool isExternal, asset_flags_type flags)
{
    if (m_data.size() + paths.size() >= m_maxTextureCount)
    {
        throw NcError("Cannot exceed max texture count.");
    }

    auto textures = std::vector<asset::TextureWithId>{};

    auto idsToLoad = std::vector<std::string>{};
    idsToLoad.reserve(paths.size());

    for (const auto& path : paths)
    {
        if (IsLoaded(path))
        {
            continue;
        }

        m_data.emplace(path);
        const auto fullPath = isExternal ? path : m_assetDirectory + path;
        textures.emplace_back(asset::ImportTexture(fullPath), path, flags);
        idsToLoad.push_back(path);
    }

    if (!idsToLoad.empty())
    {
        m_onUpdate.Emit(asset::TextureUpdateEventData{
            asset::UpdateAction::Load,
            std::move(idsToLoad),
            std::span<const asset::TextureWithId>{textures}
        });
    }

    return true;
}

bool TextureAssetManager::Unload(const std::string& path, asset_flags_type)
{
    if (!m_data.erase(path))
        return false;

    m_onUpdate.Emit(asset::TextureUpdateEventData{
        asset::UpdateAction::Unload,
        std::vector<std::string>{path},
        std::span<const asset::TextureWithId>{}
    });

    return true;
}

void TextureAssetManager::UnloadAll(asset_flags_type)
{
    m_data.clear();
    m_onUpdate.Emit(asset::TextureUpdateEventData{
        asset::UpdateAction::UnloadAll,
        {},
        {}
    });
    /** No need to send signal to GPU - no need to write an empty buffer to the GPU. **/
}

auto TextureAssetManager::Acquire(const std::string& path, asset_flags_type) const -> TextureView
{
    const auto index = m_data.index(path);
    NC_ASSERT(index != StringMap<uint32_t>::NullIndex, fmt::format("Asset is not loaded: '{}'", path));
    return TextureView{.index = static_cast<uint32_t>(index)};
}

bool TextureAssetManager::IsLoaded(const std::string& path, asset_flags_type) const
{
    return m_data.contains(path);
}

auto TextureAssetManager::OnUpdate() -> Signal<const asset::TextureUpdateEventData&>&
{
    return m_onUpdate;
}

auto TextureAssetManager::GetAllLoaded() const -> std::vector<std::string_view>
{
    return GetPaths(m_data.keys(), [](const auto& data)
    {
        return std::string_view{data};
    });
}
} // namespace nc
