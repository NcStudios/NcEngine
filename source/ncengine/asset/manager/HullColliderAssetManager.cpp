#include "HullColliderAssetManager.h"
#include "AssetUtilities.h"

#include "ncasset/Import.h"

namespace nc::asset
{
HullColliderAssetManager::HullColliderAssetManager(const std::string& assetDirectory)
    : m_hullColliders{},
      m_assetDirectory{assetDirectory}
{
}

bool HullColliderAssetManager::Load(const std::string& path, bool isExternal, asset_flags_type)
{
    if (IsLoaded(path))
    {
        return false;
    }

    const auto fullPath = isExternal ? path : m_assetDirectory + path;
    m_hullColliders.emplace(path, ImportConvexHull(fullPath));
    return true;
}

bool HullColliderAssetManager::Load(std::span<const std::string> paths, bool isExternal, asset_flags_type)
{
    bool anyLoaded = false;

    for(const auto& path : paths)
    {
        if (IsLoaded(path))
        {
            continue;
        }

        if (Load(path, isExternal))
        {
            anyLoaded = true;
        }
    }

    return anyLoaded;
}

bool HullColliderAssetManager::Unload(const std::string& path, asset_flags_type)
{
    return m_hullColliders.erase(path);
}

void HullColliderAssetManager::UnloadAll(asset_flags_type)
{
    m_hullColliders.clear();
}

auto HullColliderAssetManager::Acquire(const std::string& path, asset_flags_type) const -> ConvexHullView
{
    NC_ASSERT(m_hullColliders.contains(path), fmt::format("ConvexHull is not loaded: '{}'", path));
    return Acquire(m_hullColliders.hash(path));
}

auto HullColliderAssetManager::Acquire(AssetId id, asset_flags_type) const -> ConvexHullView
{
    [[maybe_unused]] const auto index = m_hullColliders.index(id);
    NC_ASSERT(index != m_hullColliders.NullIndex, fmt::format("ConvexHull is not loaded: '{}'", id));
    return ConvexHullView{
        .id = id
    };
}

bool HullColliderAssetManager::IsLoaded(const std::string& path, asset_flags_type) const
{
    return m_hullColliders.contains(path);
}

auto HullColliderAssetManager::GetAllLoaded() const -> std::vector<std::string_view>
{
    return GetPaths(m_hullColliders.keys());
}
} // namespace nc::asset
