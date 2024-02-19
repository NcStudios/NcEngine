#include "HullColliderAssetManager.h"
#include "AssetUtilities.h"

#include "ncasset/Import.h"

namespace nc
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
        m_hullColliders.emplace(path, asset::ImportHullCollider(fullPath));
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
        const auto hash = m_hullColliders.hash(path);
        const auto index = m_hullColliders.index(hash);
        NC_ASSERT(index != m_hullColliders.NullIndex, fmt::format("Asset is not loaded: '{}'", path));
        const auto& collider = m_hullColliders.at(index);
        return ConvexHullView
        {
            .id = hash,
            .vertices = std::span<const Vector3>{collider.vertices},
            .extents = collider.extents,
            .maxExtent = collider.maxExtent
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
}
