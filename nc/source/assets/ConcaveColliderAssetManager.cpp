#include "ConcaveColliderAssetManager.h"

#include <fstream>

namespace nc
{
    bool ConcaveColliderAssetManager::Load(const std::string& path)
    {
        if(IsLoaded(path))
            return false;
        
        std::ifstream file{path};
        if(!file.is_open())
            throw std::runtime_error("ConcaveColliderAssetManager - Could not open file: " + path);

        size_t triangleCount;
        float maxExtent;
        file >> triangleCount >> maxExtent;

        std::vector<Triangle> triangles;
        triangles.reserve(triangleCount);
        Vector3 a, b, c;

        for(size_t i = 0u; i < triangleCount; ++i)
        {
            if(file.fail())
                throw std::runtime_error("ConcaveColliderAssetManager - Failure reading file: " + path);

            file >> a.x >> a.y >> a.z
                 >> b.x >> b.y >> b.z
                 >> c.x >> c.y >> c.z;

            triangles.emplace_back(a, b, c);
        }

        m_concaveColliders.emplace(path, ConcaveColliderFlyweight{std::move(triangles), maxExtent});
        return true;
    }
    
    bool ConcaveColliderAssetManager::Load(const std::vector<std::string>& paths)
    {
        /** @todo */
        (void)paths;
        return false;
    }
    
    bool ConcaveColliderAssetManager::Unload(const std::string& path)
    {
        /** @todo */
        (void)path;
        return false;
    }

    auto ConcaveColliderAssetManager::Acquire(const std::string& path) const -> ConcaveColliderView
    {
        const auto it = m_concaveColliders.find(path);
        if(it == m_concaveColliders.end())
            throw std::runtime_error("ConcaveColliderAssetManager::Acquire - asset is not loaded: " + path);
        
        return ConcaveColliderView
        {
            .triangles = std::span<const Triangle>{it->second.triangles},
            .maxExtent = it->second.maxExtent
        };
    }
    
    bool ConcaveColliderAssetManager::IsLoaded(const std::string& path) const
    {
        return m_concaveColliders.end() != m_concaveColliders.find(path);
    }
}