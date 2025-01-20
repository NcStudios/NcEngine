#include "MeshColliderAssetManager.h"
#include "AssetUtilities.h"

#include "ncasset/Import.h"
#include "ncengine/asset/AssetData.h"

namespace nc::asset
{
MeshColliderAssetManager::MeshColliderAssetManager(const std::string& meshColliderAssetDirectory)
    : m_assetDirectory{meshColliderAssetDirectory}
{
}

bool MeshColliderAssetManager::Load(const std::string& path, bool isExternal, asset_flags_type)
{
    if (IsLoaded(path))
    {
        return false;
    }

    const auto fullPath = isExternal ? path : m_assetDirectory + path;
    m_map.emplace(path);
    const auto id = m_map.hash(path);
    auto asset = ImportMeshCollider(fullPath);
    m_onUpdate.Emit(MeshColliderUpdateEventData{
        std::span{&asset, 1},
        std::span{&id, 1},
        UpdateAction::Load
    });

    return true;
}

bool MeshColliderAssetManager::Load(std::span<const std::string> paths, bool isExternal, asset_flags_type)
{
    auto anyLoaded = false;
    auto assets = std::vector<MeshCollider>{};
    auto ids = std::vector<AssetId>{};
    assets.reserve(paths.size());
    ids.reserve(paths.size());

    for(const auto& path : paths)
    {
        if (IsLoaded(path))
        {
            continue;
        }

        anyLoaded = true;
        const auto fullPath = isExternal ? path : m_assetDirectory + path;
        assets.push_back(ImportMeshCollider(fullPath));
        ids.push_back(m_map.hash(path));
        m_map.emplace(path);
    }

    if (anyLoaded)
    {
        m_onUpdate.Emit(MeshColliderUpdateEventData{
            assets,
            ids,
            UpdateAction::Load
        });
    }

    return anyLoaded;
}

bool MeshColliderAssetManager::Unload(const std::string& path, asset_flags_type)
{
    if (m_map.erase(path))
    {
        const auto id = m_map.hash(path);
        m_onUpdate.Emit(MeshColliderUpdateEventData{
            {},
            {&id, 1},
            UpdateAction::Unload
        });

        return true;
    }

    return false;
}

void MeshColliderAssetManager::UnloadAll(asset_flags_type)
{
    m_map.clear();
    m_onUpdate.Emit(MeshColliderUpdateEventData{
        {},
        {},
        UpdateAction::UnloadAll
    });
}

auto MeshColliderAssetManager::Acquire(const std::string& path, asset_flags_type) const -> MeshColliderView
{
    NC_ASSERT(m_map.contains(path), fmt::format("MeshCollider is not loaded: '{}'", path));
    return Acquire(m_map.hash(path));
}

auto MeshColliderAssetManager::Acquire(AssetId id, asset_flags_type) const -> MeshColliderView
{
    NC_ASSERT(m_map.index(id) != m_map.NullIndex, fmt::format("MeshCollider is not loaded: '{}'", id));
    return MeshColliderView{id};
}

bool MeshColliderAssetManager::IsLoaded(const std::string& path, asset_flags_type) const
{
    return m_map.contains(path);
}

auto MeshColliderAssetManager::GetAllLoaded() const -> std::vector<std::string_view>
{
    return GetPaths(m_map.keys());
}
} // namespace nc::asset
