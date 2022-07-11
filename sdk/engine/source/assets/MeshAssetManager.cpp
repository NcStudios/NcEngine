#include "MeshAssetManager.h"
#include "AssetUtilities.h"

#include <cassert>
#include <fstream>

namespace
{
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

void ReadVerticesFromAsset(std::ifstream& file, std::vector<nc::Vertex>& vertices, size_t count)
{
    vertices.reserve(vertices.size() + count);
    nc::Vector3 ver, nrm, tan, bit;
    nc::Vector2 tex;

    for(size_t i = 0; i < count; ++i)
    {
        if(file.fail())
            throw nc::NcError("Failure reading file");
        
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
            throw nc::NcError("Failure reading file");

        file >> index;
        indices.push_back(index);
    }
}

auto ReadMesh(const std::string& meshPath, std::vector<nc::Vertex>& vertices, std::vector<uint32_t>& indices) -> MeshReadData
{
    if(!nc::HasValidAssetExtension(meshPath))
        throw nc::NcError("Invalid extension: " + meshPath);

    std::ifstream file{meshPath};
    if(!file.is_open())
        throw nc::NcError("Could not open file: " + meshPath);

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
MeshAssetManager::MeshAssetManager(const std::string& assetDirectory)
    : m_vertexData{},
      m_indexData{},
      m_accessors{},
      m_assetDirectory{assetDirectory},
      m_onMeshAdd{}
{
}

MeshAssetManager::~MeshAssetManager() noexcept
{
    m_vertexData = {};
    m_indexData = {};
    m_accessors.clear();
}

bool MeshAssetManager::Load(const std::string& path, bool isExternal)
{
    if(IsLoaded(path))
        return false;

    const auto existingVertexCount = static_cast<uint32_t>(m_vertexData.size());
    const auto existingIndexCount = static_cast<uint32_t>(m_indexData.size());

    const auto fullPath = isExternal ? path : m_assetDirectory + path;
    auto [verticesRead, indicesRead, maxExtent] = ReadMesh(fullPath, m_vertexData, m_indexData);

    MeshView mesh
    {
        .firstVertex = existingVertexCount,
        .vertexCount = verticesRead,
        .firstIndex = existingIndexCount,
        .indexCount = indicesRead,
        .maxExtent = maxExtent
    };

    m_accessors.emplace(path, mesh);

    MeshBufferData assetData
    {
        m_vertexData,
        m_indexData
    };

    m_onMeshAdd.Emit(assetData);
    return true;
}

bool MeshAssetManager::Load(std::span<const std::string> paths, bool isExternal)
{
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;
    std::unordered_map<std::string, MeshView> meshes;

    auto totalVertexCount = static_cast<uint32_t>(m_vertexData.size());
    auto totalIndexCount = static_cast<uint32_t>(m_indexData.size());

    for (const auto& path : paths)
    {
        if(IsLoaded(path))
            continue;

        const auto fullPath = isExternal ? path : m_assetDirectory + path;
        auto [verticesRead, indicesRead, maxExtent] = ReadMesh(fullPath, m_vertexData, m_indexData);

        MeshView mesh
        {
            .firstVertex = totalVertexCount,
            .vertexCount = verticesRead,
            .firstIndex = totalIndexCount,
            .indexCount = indicesRead,
            .maxExtent = maxExtent
        };

        m_accessors.emplace(path, mesh);
        totalVertexCount += verticesRead;
        totalIndexCount += indicesRead;
    }

    MeshBufferData assetData
    {
        m_vertexData,
        m_indexData
    };

    m_onMeshAdd.Emit(assetData);
    return true;
}

bool MeshAssetManager::Unload(const std::string& path)
{
    auto pos = m_accessors.find(path);
    if(pos == m_accessors.end())
        return false;

    const auto [firstVertex, vertexCount, firstIndex, indexCount, unused] = pos->second;
    m_accessors.erase(path);

    auto indBeg = m_indexData.begin() + firstIndex;
    auto indEnd = indBeg + indexCount;
    assert(indEnd <= m_indexData.end());
    m_indexData.erase(indBeg, indEnd);

    auto vertBeg = m_vertexData.begin() + firstVertex;
    auto vertEnd = vertBeg + vertexCount;
    assert(vertEnd <= m_vertexData.end());
    m_vertexData.erase(vertBeg, vertEnd);

    for(auto& [unused, accessor] : m_accessors)
    {
        if(accessor.firstVertex > firstVertex)
            accessor.firstVertex -= vertexCount;

        if(accessor.firstIndex > firstIndex)
            accessor.firstIndex -= indexCount;
    }

    if(m_vertexData.size() != 0)
    {
        MeshBufferData assetData
        {
            m_vertexData,
            m_indexData
        };

        m_onMeshAdd.Emit(assetData);
    }
    return true;
}

void MeshAssetManager::UnloadAll()
{
    /** We don't want to emit a signal with empty data. **/
    m_accessors.clear();
    m_vertexData.clear();
    m_indexData.clear();
}

auto MeshAssetManager::Acquire(const std::string& path) const -> MeshView
{
    const auto it = m_accessors.find(path);
    if(it == m_accessors.cend())
        throw NcError("Asset not loaded: " + path);
    
    return it->second;
}

bool MeshAssetManager::IsLoaded(const std::string& path) const
{
    return m_accessors.contains(path);
}

auto MeshAssetManager::OnMeshAdd() -> Signal<const MeshBufferData&>*
{
    return &m_onMeshAdd;
}
}