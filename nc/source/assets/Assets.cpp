#include "Assets.h"
#include "AssetManager.h"
#include "audio/audio_file/AudioFile.h"

#include <fstream>

namespace nc
{
    std::ifstream& operator >>(std::ifstream& stream, Vector3& vec)
    {
        stream >> vec.x >> vec.y >> vec.z;
        return stream;
    }

    void LoadSoundClipAsset(const std::string& path)
    {
        AudioFile<double> asset;
        if(!asset.load(path))
            throw std::runtime_error("LoadSoundClipAsset - Could not open file: " + path);

        size_t samplesPerChannel = asset.samples.at(0).size();

        SoundClipFlyweight data
        {
            .leftChannel = std::move(asset.samples.at(0)),
            .rightChannel = std::move(asset.samples.at(1)),
            .samplesPerChannel = samplesPerChannel
        };

        AssetManager::LoadSoundClip(path, std::move(data));
    }

    void LoadConvexHullAsset(const std::string& path)
    {
        std::ifstream file{path};
        if(!file.is_open())
            throw std::runtime_error("LoadConvexHullAsset - Could not open file: " + path);
        
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
                throw std::runtime_error("LoadConvexHullAsset - Failure reading file: " + path);

            file >> vertex;
            vertices.push_back(vertex);
        }

        AssetManager::LoadConvexHull(path, ConvexHullFlyweight{std::move(vertices), extents, maxExtent});
    }

    void LoadMeshColliderAsset(const std::string& path)
    {
        std::ifstream file{path};
        if(!file.is_open())
            throw std::runtime_error("LoadMeshColliderAsset - Could not open file: " + path);

        size_t triangleCount;
        float maxExtent;
        file >> triangleCount >> maxExtent;

        std::vector<Triangle> triangles;
        triangles.reserve(triangleCount);
        Vector3 a, b, c;

        for(size_t i = 0u; i < triangleCount; ++i)
        {
            if(file.fail())
                throw std::runtime_error("LoadMeshColliderAsset - Failure reading file: " + path);

            file >> a >> b >> c;
            triangles.emplace_back(a, b, c);
        }

        AssetManager::LoadMeshCollider(path, MeshColliderFlyweight{std::move(triangles), maxExtent});
    }
} // namespace nc