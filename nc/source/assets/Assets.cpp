#include "Assets.h"
#include "AssetManager.h"

#include <fstream>

namespace nc
{
    std::ifstream& operator >>(std::ifstream& stream, Vector3& vec)
    {
        stream >> vec.x >> vec.y >> vec.z;
        return stream;
    }

    void LoadHullColliderAsset(const std::string& path)
    {
        std::ifstream file{path};
        if(!file.is_open())
            throw std::runtime_error("LoadHullColliderAsset - Could not open file: " + path);
        
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
                throw std::runtime_error("LoadHullColliderAsset - Failure reading file: " + path);

            file >> vertex;
            vertices.push_back(vertex);
        }

        AssetManager::LoadHullCollider(path, HullColliderFlyweight{std::move(vertices), extents, maxExtent});
    }

    void LoadMeshColliderAsset(const std::string& path)
    {
        std::ifstream file{path};
        if(!file.is_open())
            throw std::runtime_error("LoadHullColliderAsset - Could not open file: " + path);

        size_t triangleCount;
        file >> triangleCount;

        std::vector<MeshCollider::Triangle> triangles;
        triangles.reserve(triangleCount);
        Vector3 a, b, c, n;

        for(size_t i = 0u; i < triangleCount; ++i)
        {
            if(file.fail())
                throw std::runtime_error("LoadMeshColliderAsset - Failure reading file: " + path);

            file >> a >> b >> c >> n;
            triangles.emplace_back(a, b, c, n);
        }

        AssetManager::LoadMeshCollider(path, MeshColliderFlyweight{std::move(triangles)});
    }
} // namespace nc