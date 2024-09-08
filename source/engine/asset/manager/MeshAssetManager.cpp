#include "MeshAssetManager.h"
#include "AssetUtilities.h"
#include "asset/AssetData.h"

#include "ncasset/Import.h"

#include <cassert>
#include <fstream>

namespace nc::asset
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

auto MeshAssetManager::ImportMesh(const std::string& path, bool isExternal) -> asset::Mesh
{
    const auto fullPath = isExternal ? path : m_assetDirectory + path;
    const auto mesh = asset::ImportMesh(fullPath);

    auto meshView = MeshView{
        .id = m_accessors.hash(path),
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
        auto& bones = mesh.bonesData.value();
        m_onBoneUpdate.Emit(BoneUpdateEventData{
            std::span<const BonesData>{&bones, 1},
            std::vector<std::string>{path},
            UpdateAction::Load
        });
    }

    m_onMeshUpdate.Emit(MeshUpdateEventData{m_vertexData, m_indexData});
    return true;
}

bool MeshAssetManager::Load(std::span<const std::string> paths, bool isExternal, asset_flags_type)
{
    auto idsToLoad = std::vector<std::string>{};
    auto bones = std::vector<BonesData>{};
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
            bones.push_back(std::move(mesh.bonesData.value()));
            idsToLoad.push_back(path);
            anyBonesLoaded = true;
        }

        anyLoaded = true;
    }

    if (anyBonesLoaded)
    {
        m_onBoneUpdate.Emit(BoneUpdateEventData{
            std::span<const BonesData>{bones},
            std::move(idsToLoad),
            UpdateAction::Load
        });
    }

    m_onMeshUpdate.Emit(MeshUpdateEventData{m_vertexData, m_indexData});
    return anyLoaded;
}

bool MeshAssetManager::Unload(const std::string& path, asset_flags_type)
{
    const auto index = m_accessors.index(path);
    if (index == StringTable::NullIndex)
        return false;

    const auto [id, firstVertex, vertexCount, firstIndex, indexCount, unused] = m_accessors.at(index);
    m_accessors.erase(path);

    auto indBeg = m_indexData.begin() + firstIndex;
    auto indEnd = indBeg + indexCount;
    assert(indEnd <= m_indexData.end());
    m_indexData.erase(indBeg, indEnd);

    auto vertBeg = m_vertexData.begin() + firstVertex;
    auto vertEnd = vertBeg + vertexCount;
    assert(vertEnd <= m_vertexData.end());
    m_vertexData.erase(vertBeg, vertEnd);

    for(auto& accessor : m_accessors)
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
            MeshUpdateEventData
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
}

auto MeshAssetManager::Acquire(const std::string& path, asset_flags_type) const -> MeshView
{
    return m_accessors.at(path);
}

bool MeshAssetManager::IsLoaded(const std::string& path, asset_flags_type) const
{
    return m_accessors.contains(path);
}

auto MeshAssetManager::GetAllLoaded() const -> std::vector<std::string_view>
{
    return GetPaths(m_accessors.keys());
}

auto MeshAssetManager::OnMeshUpdate() -> Signal<const asset::MeshUpdateEventData&>&
{
    return m_onMeshUpdate;
}

auto MeshAssetManager::OnBoneUpdate() -> Signal<const asset::BoneUpdateEventData&>&
{
    return m_onBoneUpdate;
}

auto MeshAssetManager::GetMeshData(std::string_view path) const -> NamedMesh
{
    const auto& view = m_accessors.at(path);
    return NamedMesh{
        std::span<const asset::MeshVertex>{
            m_vertexData.begin() + view.firstVertex,
            view.vertexCount
        },
        std::span<const uint32_t>{
            m_indexData.begin() + view.firstIndex,
            view.indexCount
        },
        std::string{path}
    };
}
} // namespace nc::asset
