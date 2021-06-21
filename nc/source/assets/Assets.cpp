#include "Assets.h"
#include "HullColliderManager.h"

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
        
        float maxExtent = 0.0f;
        size_t vertexCount = 0u;
        file >> maxExtent >> vertexCount;
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

        HullColliderManager::Load(path, HullColliderFlyweight{std::move(vertices), maxExtent});
    }
} // namespace nc