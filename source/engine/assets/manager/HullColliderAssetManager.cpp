#include "HullColliderAssetManager.h"

#include "ncasset/Import.h"

namespace nc
{
    HullColliderAssetManager::HullColliderAssetManager(const std::string& assetDirectory)
        : m_hullColliders{},
          m_assetDirectory{assetDirectory}
    {
    }

    bool HullColliderAssetManager::Load(const std::string& path, bool isExternal, asset_flags_type flags)
    {
        if (IsLoaded(path))
        {
            return false;
        }

        const auto fullPath = isExternal ? path : m_assetDirectory + path;
        m_hullColliders.emplace(path, asset::ImportHullCollider(fullPath));
        return true;
    }

    bool HullColliderAssetManager::Load(std::span<const std::string> paths, bool isExternal, asset_flags_type flags)
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

    bool HullColliderAssetManager::Unload(const std::string& path, asset_flags_type flags)
    {
        return static_cast<bool>(m_hullColliders.erase(path));
    }

    void HullColliderAssetManager::UnloadAll(asset_flags_type flags)
    {
        m_hullColliders.clear();
    }

    auto HullColliderAssetManager::Acquire(const std::string& path, asset_flags_type flags) const -> ConvexHullView
    {
        const auto it = m_hullColliders.find(path);
        if (it == m_hullColliders.end())
        {
            throw NcError("Asset is not loaded: " + path);
        }

        return ConvexHullView
        {
            .vertices = std::span<const Vector3>{it->second.vertices},
            .extents = it->second.extents,
            .maxExtent = it->second.maxExtent
        };
    }

    bool HullColliderAssetManager::IsLoaded(const std::string& path, asset_flags_type flags) const
    {
        return m_hullColliders.contains(path);
    }
}