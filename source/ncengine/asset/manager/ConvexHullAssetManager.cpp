#include "ConvexHullAssetManager.h"
#include "AssetUtilities.h"

#include "ncasset/Import.h"
#include "ncengine/asset/AssetData.h"

namespace nc::asset
{
ConvexHullAssetManager::ConvexHullAssetManager(const std::string& assetDirectory)
    : m_assetDirectory{assetDirectory}
{
}

bool ConvexHullAssetManager::Load(const std::string& path, bool isExternal, asset_flags_type)
{
    if (IsLoaded(path))
    {
        return false;
    }

    const auto fullPath = isExternal ? path : m_assetDirectory + path;
    m_map.emplace(path);
    const auto id = m_map.hash(path);
    auto asset = ImportConvexHull(fullPath);
    m_onUpdate.Emit(ConvexHullUpdateEventData{
        std::span{&asset, 1},
        std::span{&id, 1},
        UpdateAction::Load
    });

    return true;
}

bool ConvexHullAssetManager::Load(std::span<const std::string> paths, bool isExternal, asset_flags_type)
{
    auto anyLoaded = false;
    auto assets = std::vector<ConvexHull>{};
    auto ids = std::vector<AssetId>{};
    assets.reserve(paths.size());
    ids.reserve(paths.size());

    for(const auto& path : paths)
    {
        if (IsLoaded(path))
        {
            continue;
        }

        anyLoaded = true;
        const auto fullPath = isExternal ? path : m_assetDirectory + path;
        assets.push_back(ImportConvexHull(fullPath));
        ids.push_back(m_map.hash(path));
        m_map.emplace(path);
    }

    if (anyLoaded)
    {
        m_onUpdate.Emit(ConvexHullUpdateEventData{
            assets,
            ids,
            UpdateAction::Load
        });
    }

    return anyLoaded;
}

bool ConvexHullAssetManager::Unload(const std::string& path, asset_flags_type)
{
    if (m_map.erase(path))
    {
        const auto id = m_map.hash(path);
        m_onUpdate.Emit(ConvexHullUpdateEventData{
            {},
            {&id, 1},
            UpdateAction::Unload
        });

        return true;
    }

    return false;
}

void ConvexHullAssetManager::UnloadAll(asset_flags_type)
{
    m_map.clear();
    m_onUpdate.Emit(ConvexHullUpdateEventData{
        {},
        {},
        UpdateAction::UnloadAll
    });
}

auto ConvexHullAssetManager::Acquire(const std::string& path, asset_flags_type) const -> ConvexHullView
{
    NC_ASSERT(m_map.contains(path), fmt::format("ConvexHull is not loaded: '{}'", path));
    return Acquire(m_map.hash(path));
}

auto ConvexHullAssetManager::Acquire(AssetId id, asset_flags_type) const -> ConvexHullView
{
    NC_ASSERT(m_map.index(id) != m_map.NullIndex, fmt::format("ConvexHull is not loaded: '{}'", id));
    return ConvexHullView{id};
}

bool ConvexHullAssetManager::IsLoaded(const std::string& path, asset_flags_type) const
{
    return m_map.contains(path);
}

auto ConvexHullAssetManager::GetAllLoaded() const -> std::vector<std::string_view>
{
    return GetPaths(m_map.keys());
}
} // namespace nc::asset
