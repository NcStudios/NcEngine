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

    void LoadSoundClipAsset(const std::filesystem::path& path)
    {
        AudioFile<double> asset;
        if(!asset.load(path.string()))
            throw std::runtime_error("LoadSoundClipAsset - Could not open file: " + path.string());

        size_t samplesPerChannel = asset.samples.at(0).size();

        SoundClipFlyweight data
        {
            .leftChannel = std::move(asset.samples.at(0)),
            .rightChannel = std::move(asset.samples.at(1)),
            .samplesPerChannel = samplesPerChannel
        };

        AssetManager::LoadSoundClip(path.string(), std::move(data));
    }

    void LoadConvexHullAsset(const std::filesystem::path& path)
    {
        std::ifstream file{path};
        if(!file.is_open())
            throw std::runtime_error("LoadConvexHullAsset - Could not open file: " + path.string());
        
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
                throw std::runtime_error("LoadConvexHullAsset - Failure reading file: " + path.string());

            file >> vertex;
            vertices.push_back(vertex);
        }

        AssetManager::LoadConvexHull(path.string(), ConvexHullFlyweight{std::move(vertices), extents, maxExtent});
    }

    void LoadConcaveColliderAsset(const std::filesystem::path& path)
    {
        std::ifstream file{path};
        if(!file.is_open())
            throw std::runtime_error("LoadConcaveColliderAsset - Could not open file: " + path.string());

        size_t triangleCount;
        float maxExtent;
        file >> triangleCount >> maxExtent;

        std::vector<Triangle> triangles;
        triangles.reserve(triangleCount);
        Vector3 a, b, c;

        for(size_t i = 0u; i < triangleCount; ++i)
        {
            if(file.fail())
                throw std::runtime_error("LoadConcaveColliderAsset - Failure reading file: " + path.string());

            file >> a >> b >> c;
            triangles.emplace_back(a, b, c);
        }

        AssetManager::LoadConcaveCollider(path.string(), ConcaveColliderFlyweight{std::move(triangles), maxExtent});
    }
} // namespace nc