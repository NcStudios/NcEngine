#include "ConvexHullAssetManager.h"

#include <fstream>

namespace nc
{
    bool ConvexHullAssetManager::Load(const std::string& path)
    {
        if(IsLoaded(path))
            return false;
        
        std::ifstream file{path};
        if(!file.is_open())
            throw NcError("Failure opening file: " + path);
        
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
                throw NcError("Failure reading file: " + path);

            file >> vertex.x >> vertex.y >> vertex.z;
            vertices.push_back(vertex);
        }

        m_hullColliders.emplace(path, ConvexHullFlyweight{std::move(vertices), extents, maxExtent});
        return true;
    }

    bool ConvexHullAssetManager::Load(const std::vector<std::string>& paths)
    {
        /** @todo */
        (void)paths;
        return false;
    }

    bool ConvexHullAssetManager::Unload(const std::string& path)
    {
        /** @todo */
        (void)path;
        return false;
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