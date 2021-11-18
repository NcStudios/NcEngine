#include "ConvexHullAssetManager.h"

#include <fstream>

namespace nc
{
    ConvexHullAssetManager::ConvexHullAssetManager(const std::string& assetDirectory)
        : m_hullColliders{},
          m_assetDirectory{assetDirectory}
    {
    }

    bool ConvexHullAssetManager::Load(const std::string& path, bool isExternal)
    {
        if(IsLoaded(path))
            return false;

        const auto fullPath = isExternal ? path : m_assetDirectory + path;
        std::ifstream file{fullPath};
        if(!file.is_open())
            throw NcError("Failure opening file: " + fullPath);
        
        Vector3 extents;
        float maxExtent;
        size_t vertexCount;
        file >> extents.x >> extents.y >> extents.z >> maxExtent >> vertexCount;
        std::vector<Vector3> vertices;
        vertices.reserve(vertexCount);
        Vector3 vertex;

        for(size_t i = 0u; i < vertexCount; ++i)
        {
            if(file.fail())
                throw NcError("Failure reading file: " + fullPath);

            file >> vertex.x >> vertex.y >> vertex.z;
            vertices.push_back(vertex);
        }

        m_hullColliders.emplace(path, ConvexHullFlyweight{std::move(vertices), extents, maxExtent});
        return true;
    }

    bool ConvexHullAssetManager::Load(std::span<const std::string> paths, bool isExternal)
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
        if(it == m_hullColliders.end())
            throw NcError("Asset is not loaded: " + path);
        
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