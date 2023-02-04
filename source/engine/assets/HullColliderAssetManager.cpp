#include "ConvexHullAssetManager.h"

#include "ncasset/Import.h"

namespace nc
{
    ConvexHullAssetManager::ConvexHullAssetManager(const std::string& assetDirectory)
        : m_hullColliders{},
          m_assetDirectory{assetDirectory}
    {
    }

    bool ConvexHullAssetManager::Load(const std::string& path, bool isExternal)
    {
        if (IsLoaded(path))
        {
            return false;
        }

        const auto fullPath = isExternal ? path : m_assetDirectory + path;
        m_hullColliders.emplace(path, asset::ImportHullCollider(fullPath));
        return true;
    }

    bool ConvexHullAssetManager::Load(std::span<const std::string> paths, bool isExternal)
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

    bool ConvexHullAssetManager::Unload(const std::string& path)
    {
        return static_cast<bool>(m_hullColliders.erase(path));
    }

    void ConvexHullAssetManager::UnloadAll()
    {
        m_hullColliders.clear();
    }

    auto ConvexHullAssetManager::Acquire(const std::string& path) const -> ConvexHullView
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

    bool ConvexHullAssetManager::IsLoaded(const std::string& path) const
    {
        return m_hullColliders.contains(path);
    }
}