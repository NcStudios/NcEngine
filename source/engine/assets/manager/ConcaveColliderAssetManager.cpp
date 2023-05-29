#include "ConcaveColliderAssetManager.h"

#include "ncasset/Import.h"

namespace nc
{
    ConcaveColliderAssetManager::ConcaveColliderAssetManager(const std::string& concaveColliderAssetDirectory)
        : m_concaveColliders{},
          m_assetDirectory{concaveColliderAssetDirectory}
    {
    }

    bool ConcaveColliderAssetManager::Load(const std::string& path, bool isExternal, asset_flags_type)
    {
        if (IsLoaded(path))
        {
            return false;
        }

        const auto fullPath = isExternal ? path : m_assetDirectory + path;
        m_concaveColliders.emplace(path, asset::ImportConcaveCollider(fullPath));
        return true;
    }
    
    bool ConcaveColliderAssetManager::Load(std::span<const std::string> paths, bool isExternal, asset_flags_type)
    {
        auto anyLoaded = false;

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
    
    bool ConcaveColliderAssetManager::Unload(const std::string& path, asset_flags_type)
    {
        return static_cast<bool>(m_concaveColliders.erase(path));
    }

    void ConcaveColliderAssetManager::UnloadAll(asset_flags_type)
    {
        m_concaveColliders.clear();
    }

    auto ConcaveColliderAssetManager::Acquire(const std::string& path, asset_flags_type) const -> ConcaveColliderView
    {
        const auto it = m_concaveColliders.find(path);
        if (it == m_concaveColliders.end())
        {
            throw NcError("Asset is not loaded: " + path);
        }

        return ConcaveColliderView
        {
            .triangles = std::span<const Triangle>{it->second.triangles},
            .maxExtent = it->second.maxExtent
        };
    }
    
    bool ConcaveColliderAssetManager::IsLoaded(const std::string& path, asset_flags_type) const
    {
        return m_concaveColliders.end() != m_concaveColliders.find(path);
    }
}