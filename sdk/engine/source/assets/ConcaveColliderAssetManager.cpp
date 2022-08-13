#include "ConcaveColliderAssetManager.h"

#include <fstream>

namespace nc
{
    ConcaveColliderAssetManager::ConcaveColliderAssetManager(const std::string& concaveColliderAssetDirectory)
        : m_concaveColliders{},
          m_assetDirectory{concaveColliderAssetDirectory}
    {
    }

    bool ConcaveColliderAssetManager::Load(const std::string& path, bool isExternal)
    {
        if(IsLoaded(path))
            return false;
        
        const auto fullPath = isExternal ? path : m_assetDirectory + path;

        std::ifstream file{fullPath};
        if(!file.is_open())
            throw NcError("Failure opening file: " + fullPath);

        size_t triangleCount;
        float maxExtent;
        file >> triangleCount >> maxExtent;

        std::vector<physics::Triangle> triangles;
        triangles.reserve(triangleCount);
        Vector3 a, b, c;

        for(size_t i = 0u; i < triangleCount; ++i)
        {
            if(file.fail())
                throw NcError("Failure reading file: " + fullPath);

            file >> a.x >> a.y >> a.z
                 >> b.x >> b.y >> b.z
                 >> c.x >> c.y >> c.z;

            triangles.emplace_back(a, b, c);
        }

        m_concaveColliders.emplace(path, ConcaveColliderFlyweight{std::move(triangles), maxExtent});
        return true;
    }
    
    bool ConcaveColliderAssetManager::Load(std::span<const std::string> paths, bool isExternal)
    {
        bool anyLoaded = false;

        for(const auto& path : paths)
        {
            if(IsLoaded(path))
                continue;
            
            if(Load(path, isExternal))
                anyLoaded = true;
        }

        return anyLoaded;
    }
    
    bool ConcaveColliderAssetManager::Unload(const std::string& path)
    {
        return static_cast<bool>(m_concaveColliders.erase(path));
    }

    void ConcaveColliderAssetManager::UnloadAll()
    {
        m_concaveColliders.clear();
    }

    auto ConcaveColliderAssetManager::Acquire(const std::string& path) const -> ConcaveColliderView
    {
        const auto it = m_concaveColliders.find(path);
        if(it == m_concaveColliders.end())
            throw NcError("Asset is not loaded: " + path);
        
        return ConcaveColliderView
        {
            .triangles = std::span<const physics::Triangle>{it->second.triangles},
            .maxExtent = it->second.maxExtent
        };
    }
    
    bool ConcaveColliderAssetManager::IsLoaded(const std::string& path) const
    {
        return m_concaveColliders.end() != m_concaveColliders.find(path);
    }
}