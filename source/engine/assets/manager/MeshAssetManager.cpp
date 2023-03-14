#include "MeshAssetManager.h"
#include "asset/AssetData.h"
#include "assets/AssetUtilities.h"

#include "ncasset/Import.h"

#include <cassert>
#include <fstream>

namespace nc
{
MeshAssetManager::MeshAssetManager(const std::string& assetDirectory)
    : m_vertexData{},
      m_indexData{},
      m_accessors{},
      m_assetDirectory{assetDirectory},
      m_onUpdate{}
{
}

MeshAssetManager::~MeshAssetManager() noexcept
{
    m_vertexData = {};
    m_indexData = {};
    m_accessors.clear();
}

void MeshAssetManager::AddMeshView(const std::string& path, bool isExternal)
{
    const auto fullPath = isExternal ? path : m_assetDirectory + path;
    const auto mesh = asset::ImportMesh(fullPath);

    auto meshView = MeshView{
        .firstVertex = static_cast<uint32_t>(m_vertexData.size()),
        .vertexCount = static_cast<uint32_t>(mesh.vertices.size()),
        .firstIndex = static_cast<uint32_t>(m_indexData.size()),
        .indexCount = static_cast<uint32_t>(mesh.indices.size()),
        .maxExtent = mesh.maxExtent
    };

    m_vertexData.insert(m_vertexData.end(), mesh.vertices.begin(), mesh.vertices.end());
    m_indexData.insert(m_indexData.end(), mesh.indices.begin(), mesh.indices.end());
    m_accessors.emplace(path, meshView);
}

bool MeshAssetManager::Load(const std::string& path, bool isExternal)
{
    if (IsLoaded(path))
    {
        return false;
    }

    AddMeshView(path, isExternal);
    m_onUpdate.Emit(MeshBufferData{m_vertexData, m_indexData});
    return true;
}

bool MeshAssetManager::Load(std::span<const std::string> paths, bool isExternal)
{
    for (const auto& path : paths)
    {
        if (IsLoaded(path))
        {
            continue;
        }

        AddMeshView(path, isExternal);
    }

    m_onUpdate.Emit(MeshBufferData{m_vertexData, m_indexData});
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
        m_onUpdate.Emit
        (
            MeshBufferData
            {
                m_vertexData,
                m_indexData
            }
        );
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

auto MeshAssetManager::OnUpdate() -> Signal<const MeshBufferData&>&
{
    return m_onUpdate;
}
} // namespace nc