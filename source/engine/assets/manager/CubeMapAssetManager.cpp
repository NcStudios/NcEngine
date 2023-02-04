#include "CubeMapAssetManager.h"
#include "assets/AssetUtilities.h"

#include "ncasset/Import.h"

#include <array>
#include <cassert>
#include <fstream>
#include <filesystem>
#include <iostream>

namespace nc
{
CubeMapAssetManager::CubeMapAssetManager(const std::string& cubeMapAssetDirectory, uint32_t maxCubeMapsCount)
    : m_cubeMapIds{},
      m_assetDirectory{cubeMapAssetDirectory},
      m_maxCubeMapsCount{maxCubeMapsCount},
      m_onUpdate{}
{
}

bool CubeMapAssetManager::Load(const std::string& path, bool isExternal)
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
    const auto data = CubeMapData{asset::ImportCubeMap(fullPath), path};
    m_cubeMapIds.push_back(path);
    m_onUpdate.Emit(CubeMapBufferData{
        UpdateAction::Load,
        std::vector<std::string>{path},
        std::span<const CubeMapData>{&data, 1}
    });

    return true;
}

bool CubeMapAssetManager::Load(std::span<const std::string> paths, bool isExternal)
{
    if (paths.size() + m_cubeMapIds.size() >= m_maxCubeMapsCount)
    {
        throw NcError("Cannot exceed max texture count.");
    }

    auto loadedCubeMaps = std::vector<CubeMapData>{};
    auto idsToLoad = std::vector<std::string>{};
    loadedCubeMaps.reserve(paths.size());
    idsToLoad.reserve(paths.size());

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
        loadedCubeMaps.push_back(CubeMapData{asset::ImportCubeMap(fullPath), path});
        idsToLoad.push_back(path);
        m_cubeMapIds.push_back(path);
    }

    if (idsToLoad.empty())
    {
        return false;
    }

    m_onUpdate.Emit(CubeMapBufferData{
        UpdateAction::Load,
        std::move(idsToLoad),
        std::span<const CubeMapData>{loadedCubeMaps}
    });

    return true;
}

bool CubeMapAssetManager::Unload(const std::string& path)
{
    if (const auto pos = std::ranges::find(m_cubeMapIds, path); pos != m_cubeMapIds.cend())
    {
        m_cubeMapIds.erase(pos);
        m_onUpdate.Emit
        (
            CubeMapBufferData(UpdateAction::Unload, std::vector<std::string>{path}, std::span<const CubeMapData>{})
        );

        return true;
    }

    return false;
}

void CubeMapAssetManager::UnloadAll()
{
    m_cubeMapIds.clear();
}

auto CubeMapAssetManager::Acquire(const std::string& path) const -> CubeMapView
{
    const auto pos = std::ranges::find(m_cubeMapIds, path);
    if (pos == m_cubeMapIds.cend())
    {
        throw NcError("Asset is not loaded: " + path);
    }

    const auto index = static_cast<uint32_t>(std::distance(m_cubeMapIds.cbegin(), pos));
    return CubeMapView{CubeMapUsage::Skybox, index};
}

bool CubeMapAssetManager::IsLoaded(const std::string& path) const
{
    return m_cubeMapIds.cend() != std::ranges::find(m_cubeMapIds, path);
}

auto CubeMapAssetManager::OnUpdate() -> Signal<const CubeMapBufferData&>*
{
    return &m_onUpdate;
}
}