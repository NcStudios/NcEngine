#include "MeshAssetManager.h"

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
    MeshAssetManager::MeshAssetManager(graphics::Graphics* graphics)
        : m_graphics{graphics},
          m_vertexData{},
          m_indexData{},
          m_accessors{}
    {
    }

    MeshAssetManager::~MeshAssetManager() noexcept
    {
        m_vertexData = {};
        m_indexData = {};
        m_accessors.clear();
    }

    bool MeshAssetManager::Load(const std::string& path)
    {
        if(IsLoaded(path))
            return false;

        const auto existingVertexCount = static_cast<uint32_t>(m_vertexData.vertices.size());
        const auto existingIndexCount = static_cast<uint32_t>(m_indexData.indices.size());

        auto [verticesRead, indicesRead, maxExtent] = ReadMesh(path, m_vertexData.vertices, m_indexData.indices);

        MeshView mesh
        {
            .firstVertex = existingVertexCount,
            .firstIndex = existingIndexCount,
            .indicesCount = indicesRead,
            .maxExtent = maxExtent
        };

        m_accessors.emplace(path, mesh);
        UpdateBuffers();
        return true;
    }

    bool MeshAssetManager::Load(const std::vector<std::string>& paths)
    {
        std::vector<Vertex> vertices;
        std::vector<uint32_t> indices;
        std::unordered_map<std::string, MeshView> meshes;

        auto totalVertexCount = static_cast<uint32_t>(m_vertexData.vertices.size());
        auto totalIndexCount = static_cast<uint32_t>(m_indexData.indices.size());

        for (const auto& path : paths)
        {
            if(IsLoaded(path))
                continue;

            auto [verticesRead, indicesRead, maxExtent] = ReadMesh(path, m_vertexData.vertices, m_indexData.indices);

            MeshView mesh
            {
                .firstVertex = totalVertexCount,
                .firstIndex = totalIndexCount,
                .indicesCount = indicesRead,
                .maxExtent = maxExtent
            };

            m_accessors.emplace(path, mesh);
            totalVertexCount += verticesRead;
            totalIndexCount += indicesRead;
        }

        UpdateBuffers();
        return true;
    }

    bool MeshAssetManager::Unload(const std::string& path)
    {
        /** @todo */
        (void)path;
        return false;
    }

    auto MeshAssetManager::Acquire(const std::string& path) const -> MeshView
    {
        const auto it = m_accessors.find(path);
        if(it == m_accessors.cend())
            throw std::runtime_error("MeshAssetManager::Acquire - asset not loaded: " + path);
        
        return it->second;
    }

    bool MeshAssetManager::IsLoaded(const std::string& path) const
    {
        return m_accessors.contains(path);
    }

    void MeshAssetManager::UpdateBuffers()
    {
        m_vertexData.buffer.Clear();
        m_vertexData.buffer = ImmutableBuffer<graphics::Vertex>(m_graphics, m_vertexData.vertices);

        m_indexData.buffer.Clear();
        m_indexData.buffer = ImmutableBuffer<uint32_t>(m_graphics, m_indexData.indices);
    }
}