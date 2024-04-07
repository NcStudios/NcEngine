#include "CubeMapAssetManager.h"
#include "AssetUtilities.h"
#include "asset/AssetData.h"

#include "ncasset/Import.h"

#include <array>
#include <cassert>
#include <fstream>
#include <filesystem>

namespace nc::asset
{
CubeMapAssetManager::CubeMapAssetManager(const std::string& cubeMapAssetDirectory, uint32_t maxCubeMapsCount)
    : m_cubeMapIds{},
      m_assetDirectory{cubeMapAssetDirectory},
      m_maxCubeMapsCount{maxCubeMapsCount},
      m_onUpdate{}
{
}

bool CubeMapAssetManager::Load(const std::string& path, bool isExternal, asset_flags_type)
{
    if (IsLoaded(path))
    {
        return false;
    }

    if (!HasValidAssetExtension(path))
    {
        throw nc::NcError("Invalid extension: " + path);
    }

    const auto fullPath = isExternal ? path : m_assetDirectory + path;
    const auto data = CubeMapWithId{ImportCubeMap(fullPath), m_cubeMapIds.hash(path)};
    m_cubeMapIds.emplace(path);
    m_onUpdate.Emit(CubeMapUpdateEventData{
        UpdateAction::Load,
        std::span<const CubeMapWithId>{&data, 1}
    });

    return true;
}

bool CubeMapAssetManager::Load(std::span<const std::string> paths, bool isExternal, asset_flags_type)
{
    if (paths.size() + m_cubeMapIds.size() >= m_maxCubeMapsCount)
    {
        throw NcError("Cannot exceed max texture count.");
    }

    auto loadedCubeMaps = std::vector<asset::CubeMapWithId>{};
    loadedCubeMaps.reserve(paths.size());

    for (const auto& path : paths)
    {
        if (IsLoaded(path))
        {
            continue;
        }

        if (!HasValidAssetExtension(path))
        {
            throw nc::NcError("Invalid extension: " + path);
        }

        const auto fullPath = isExternal ? path : m_assetDirectory + path;
        loadedCubeMaps.push_back(asset::CubeMapWithId{asset::ImportCubeMap(fullPath), m_cubeMapIds.hash(path)});
        m_cubeMapIds.emplace(path);
    }

    if (loadedCubeMaps.empty())
    {
        return false;
    }

    m_onUpdate.Emit(CubeMapUpdateEventData{
        UpdateAction::Load,
        std::span<const CubeMapWithId>{loadedCubeMaps}
    });

    return true;
}

bool CubeMapAssetManager::Unload(const std::string& path, asset_flags_type)
{
    if (!m_cubeMapIds.erase(path))
        return false;

    m_onUpdate.Emit(CubeMapUpdateEventData{
        UpdateAction::Unload,
        {}
    });

    return true;
}

void CubeMapAssetManager::UnloadAll(asset_flags_type)
{
    m_cubeMapIds.clear();
}

auto CubeMapAssetManager::Acquire(const std::string& path, asset_flags_type) const -> CubeMapView
{
    const auto hash = m_cubeMapIds.hash(path);
    const auto index = m_cubeMapIds.index(hash);
    NC_ASSERT(index != m_cubeMapIds.NullIndex, fmt::format("Asset is not loaded: '{}'", path));
    return CubeMapView
    {
        .id = hash,
        .usage = CubeMapUsage::Skybox,
        .index = static_cast<unsigned>(index)
    };
}

bool CubeMapAssetManager::IsLoaded(const std::string& path, asset_flags_type) const
{
    return m_cubeMapIds.contains(path);
}

auto CubeMapAssetManager::GetAllLoaded() const -> std::vector<std::string_view>
{
    return GetPaths(m_cubeMapIds.keys());
}

auto CubeMapAssetManager::OnUpdate() -> Signal<const asset::CubeMapUpdateEventData&>&
{
    return m_onUpdate;
}
} // namespace nc::asset
