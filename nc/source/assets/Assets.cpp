#include "Assets.h"
#include "AssetManager.h"
#include "audio/audio_file/AudioFile.h"
#include "graphics/Mesh.h"
#include "graphics/resources/ResourceManager.h"

#include <fstream>

namespace
{
    using namespace nc::graphics;

    struct MeshReadData
    {
        uint32_t verticesRead;
        uint32_t indicesRead;
        float meshMaxExtent;
    };

    std::ifstream& operator >>(std::ifstream& stream, nc::Vector2& vec)
    {
        stream >> vec.x >> vec.y;
        return stream;
    }

    std::ifstream& operator >>(std::ifstream& stream, nc::Vector3& vec)
    {
        stream >> vec.x >> vec.y >> vec.z;
        return stream;
    }

    bool HasValidAssetExtension(const std::string& path)
    {
        std::size_t periodPosition = path.rfind('.');
        const std::string fileExtension = path.substr(periodPosition+1);
        return fileExtension.compare("nca") == 0 ? true : false;
    }

    void ReadVerticesFromAsset(std::ifstream& file, std::vector<Vertex>& vertices, size_t count)
    {
        vertices.reserve(vertices.size() + count);
        nc::Vector3 ver, nrm, tan, bit;
        nc::Vector2 tex;

        for(size_t i = 0; i < count; ++i)
        {
            if(file.fail())
                throw std::runtime_error("ReadVerticesFromAsset - Failure");
            
            file >> ver >> nrm >> tex >> tan >> bit;
            vertices.emplace_back(ver, nrm, tex, tan, bit);
        }
    }

    void ReadIndicesFromAsset(std::ifstream& file, std::vector<uint32_t>& indices, size_t count)
    {
        indices.reserve(indices.size() + count);
        uint32_t index; //@todo: Update vulkan descriptors to accept 16_t and change here to 16_t
        for(size_t i = 0; i < count; ++i)
        {
            if(file.fail())
                throw std::runtime_error("ReadIndicesFromAsset - Failure");

            file >> index;
            indices.push_back(index);
        }
    }

    auto ReadMesh(const std::string& meshPath, std::vector<Vertex>& vertices, std::vector<uint32_t>& indices) -> MeshReadData
    {
        if(!HasValidAssetExtension(meshPath))
            throw std::runtime_error("LoadMeshes - Invalid extension: " + meshPath);

        std::ifstream file{meshPath};
        if(!file.is_open())
            throw std::runtime_error("LoadMeshes - Could not open file: " + meshPath);

        float maxExtent;
        size_t vertexCount, indexCount;
        file >> maxExtent;
        file >> vertexCount;
        ReadVerticesFromAsset(file, vertices, vertexCount);
        file >> indexCount;
        ReadIndicesFromAsset(file, indices, indexCount);

        return MeshReadData{static_cast<uint32_t>(vertexCount), static_cast<uint32_t>(indexCount), maxExtent};
    }
}

namespace nc
{
    void LoadSoundClipAsset(const std::string& path)
    {
        if(AssetManager::IsSoundClipLoaded(path))
            return;

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
        if(AssetManager::IsConvexHullLoaded(path))
            return;

        std::ifstream file{path};
        if(!file.is_open())
            throw std::runtime_error("LoadConvexHullAsset - Could not open file: " + path);
        
        Vector3 extents;
        float maxExtent;
        size_t vertexCount;
        file >> extents >> maxExtent >> vertexCount;
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

    void LoadConcaveColliderAsset(const std::string& path)
    {
        if(AssetManager::IsConcaveColliderLoaded(path))
            return;
        
        std::ifstream file{path};
        if(!file.is_open())
            throw std::runtime_error("LoadConcaveColliderAsset - Could not open file: " + path);

        size_t triangleCount;
        float maxExtent;
        file >> triangleCount >> maxExtent;

        std::vector<Triangle> triangles;
        triangles.reserve(triangleCount);
        Vector3 a, b, c;

        for(size_t i = 0u; i < triangleCount; ++i)
        {
            if(file.fail())
                throw std::runtime_error("LoadConcaveColliderAsset - Failure reading file: " + path);

            file >> a >> b >> c;
            triangles.emplace_back(a, b, c);
        }

        AssetManager::LoadConcaveCollider(path, ConcaveColliderFlyweight{std::move(triangles), maxExtent});
    }

    void LoadMesh(const std::string& meshPath)
    {
        if(ResourceManager::MeshExists(meshPath))
            return;

        std::vector<Vertex> vertices;
        std::vector<uint32_t> indices;
        auto [verticesRead, indicesRead, maxExtent] = ReadMesh(meshPath, vertices, indices);

        graphics::Mesh mesh
        {
            .firstVertex = static_cast<uint32_t>(ResourceManager::GetVertexData().vertices.size()),
            .firstIndex = static_cast<uint32_t>(ResourceManager::GetIndexData().indices.size()),
            .indicesCount = indicesRead,
            .maxExtent = maxExtent
        };

        ResourceManager::UpdateMeshes(std::move(vertices), std::move(indices), std::unordered_map<std::string, graphics::Mesh>{ {meshPath, mesh} });
    }

    void LoadMeshes(const std::vector<std::string>& meshPaths)
    {
        std::vector<Vertex> vertices;
        std::vector<uint32_t> indices;
        std::unordered_map<std::string, Mesh> meshes;

        auto totalVertexCount = static_cast<uint32_t>(ResourceManager::GetVertexData().vertices.size());
        auto totalIndexCount = static_cast<uint32_t>(ResourceManager::GetIndexData().indices.size());

        for (const auto& path : meshPaths)
        {
            if(ResourceManager::MeshExists(path))
                continue;

            auto [verticesRead, indicesRead, maxExtent] = ReadMesh(path, vertices, indices);

            graphics::Mesh mesh
            {
                .firstVertex = totalVertexCount,
                .firstIndex = totalIndexCount,
                .indicesCount = indicesRead,
                .maxExtent = maxExtent
            };

            meshes.emplace(path, mesh);
            totalVertexCount += verticesRead;
            totalIndexCount += indicesRead;
        }

        ResourceManager::UpdateMeshes(std::move(vertices), std::move(indices), std::move(meshes));
    }
} // namespace nc