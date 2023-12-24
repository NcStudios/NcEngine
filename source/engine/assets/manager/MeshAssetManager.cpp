#include "MeshAssetManager.h"
#include "AssetUtilities.h"
#include "asset/AssetData.h"

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
      m_onBoneUpdate{},
      m_onMeshUpdate{}
{
}

MeshAssetManager::~MeshAssetManager() noexcept
{
    m_vertexData = {};
    m_indexData = {};
    m_accessors.clear();
}

auto MeshAssetManager::ImportMesh(const std::string& path, bool isExternal) -> asset::Mesh
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
    return mesh;
}

bool MeshAssetManager::Load(const std::string& path, bool isExternal, asset_flags_type)
{
    if (IsLoaded(path))
    {
        return false;
    }

    auto mesh = ImportMesh(path, isExternal);
    if (mesh.bonesData.has_value() && mesh.bonesData.value().vertexSpaceToBoneSpace.size() > 0)
    {
        m_bonesData.push_back(std::move(mesh.bonesData.value()));
        m_onBoneUpdate.Emit(asset::BoneUpdateEventData{
            std::span<const asset::BonesData>{&m_bonesData.back(), 1},
            std::vector<std::string>{path},
            asset::UpdateAction::Load
        });
    }

    m_onMeshUpdate.Emit(asset::MeshUpdateEventData{m_vertexData, m_indexData});
    return true;
}

bool MeshAssetManager::Load(std::span<const std::string> paths, bool isExternal, asset_flags_type)
{
    auto idsToLoad = std::vector<std::string>{};
    idsToLoad.reserve(paths.size());
    bool anyLoaded = false;
    bool anyBonesLoaded = false;

    for (const auto& path : paths)
    {
        if (IsLoaded(path))
        {
            continue;
        }

        auto mesh = ImportMesh(path, isExternal);
        if (mesh.bonesData.has_value() && mesh.bonesData.value().vertexSpaceToBoneSpace.size() > 0)
        {
            m_bonesData.push_back(std::move(mesh.bonesData.value()));
            idsToLoad.push_back(path);
            anyBonesLoaded = true;
        }
        anyLoaded = true;
    }

    if (anyBonesLoaded)
    {
        m_onBoneUpdate.Emit(asset::BoneUpdateEventData{
            std::span<const asset::BonesData>{m_bonesData.end() - idsToLoad.size(), m_bonesData.end()},
            std::move(idsToLoad),
            asset::UpdateAction::Load
        });
    }

    m_onMeshUpdate.Emit(asset::MeshUpdateEventData{m_vertexData, m_indexData});
    return anyLoaded;
}

bool MeshAssetManager::Unload(const std::string& path, asset_flags_type)
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

    for(auto& [uid, accessor] : m_accessors)
    {
        if(accessor.firstVertex > firstVertex)
            accessor.firstVertex -= vertexCount;

        if(accessor.firstIndex > firstIndex)
            accessor.firstIndex -= indexCount;
    }

    if(m_vertexData.size() != 0)
    {
        m_onMeshUpdate.Emit
        (
            asset::MeshUpdateEventData
            {
                m_vertexData,
                m_indexData
            }
        );
    }
    return true;
}

void MeshAssetManager::UnloadAll(asset_flags_type)
{
    /** We don't want to emit a signal with empty data. **/
    m_accessors.clear();
    m_vertexData.clear();
    m_indexData.clear();
    m_bonesData.clear();
}

auto MeshAssetManager::Acquire(const std::string& path, asset_flags_type) const -> MeshView
{
    const auto it = m_accessors.find(path);
    if(it == m_accessors.cend())
        throw NcError("Asset not loaded: " + path);
    
    return it->second;
}

bool MeshAssetManager::IsLoaded(const std::string& path, asset_flags_type) const
{
    return m_accessors.contains(path);
}

auto MeshAssetManager::GetAllLoaded() const -> std::vector<std::string_view>
{
    return GetPaths(m_accessors);
}

auto MeshAssetManager::OnMeshUpdate() -> Signal<const asset::MeshUpdateEventData&>&
{
    return m_onMeshUpdate;
}

auto MeshAssetManager::OnBoneUpdate() -> Signal<const asset::BoneUpdateEventData&>&
{
    return m_onBoneUpdate;
}
} // namespace nc