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

auto CubeMapAssetManager::Load(const std::string& path) -> bool
{
    if (IsLoaded(path))
    {
        return false;
    }

    if (!HasValidAssetExtension(path))
    {
        throw nc::NcError("Invalid extension: " + path);
    }

    const auto fullPath = m_assetDirectory + path;
    const auto data = CubeMapWithId{ImportCubeMap(fullPath), m_cubeMapIds.hash(path)};
    m_cubeMapIds.emplace(path);
    m_onUpdate.Emit(CubeMapUpdateEventData{
        UpdateAction::Load,
        std::span<const CubeMapWithId>{&data, 1}
    });

    return true;
}

auto CubeMapAssetManager::Load(std::span<const std::string> paths) -> bool
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

        const auto fullPath = m_assetDirectory + path;
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

auto CubeMapAssetManager::Unload(const std::string& path) -> bool
{
    if (!m_cubeMapIds.erase(path))
        return false;

    m_onUpdate.Emit(CubeMapUpdateEventData{
        UpdateAction::Unload,
        {}
    });

    return true;
}

void CubeMapAssetManager::UnloadAll()
{
    m_cubeMapIds.clear();
}

auto CubeMapAssetManager::Acquire(const std::string& path) const -> CubeMapView
{
    NC_ASSERT(m_cubeMapIds.contains(path), fmt::format("CubeMap is not loaded: '{}'", path));
    return Acquire(m_cubeMapIds.hash(path));
}

auto CubeMapAssetManager::Acquire(AssetId id) const -> CubeMapView
{
    const auto index = m_cubeMapIds.index(id);
    NC_ASSERT(index != m_cubeMapIds.NullIndex, fmt::format("CubeMap is not loaded: '{}'", id));
    return CubeMapView{
        .id = id,
        .usage = CubeMapUsage::Skybox,
        .index = static_cast<unsigned>(index)
    };
}

auto CubeMapAssetManager::GetAllLoaded(bool serializableOnly) const -> std::vector<std::string_view>
{
    (void)serializableOnly;
    return GetPaths(m_cubeMapIds.keys());
}
} // namespace nc::asset
