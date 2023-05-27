#include "NormalMapAssetManager.h"
#include "asset/AssetData.h"

#include "ncasset/Import.h"

namespace nc
{
NormalMapAssetManager::NormalMapAssetManager(const std::string& normalMapsAssetDirectory, uint32_t maxNormalMaps)
    : m_normalMapData{},
    m_assetDirectory{ normalMapsAssetDirectory },
    m_maxNormalMapCount{ maxNormalMaps },
    m_onUpdate{}
{
    m_normalMapData.reserve(m_maxNormalMapCount);
}

bool NormalMapAssetManager::Load(const std::string& path, bool isExternal)
{
    const auto index = static_cast<uint32_t>(m_normalMapData.size());

    if (index + 1 >= m_maxNormalMapCount)
    {
        throw NcError("Cannot exceed max normal map count.");
    }

    if (IsLoaded(path))
    {
        return false;
    }

    const auto fullPath = isExternal ? path : m_assetDirectory + path;
    m_normalMapData.emplace_back(asset::ImportTexture(fullPath), path);

    m_onUpdate.Emit(asset::NormalMapUpdateEventData{
        asset::UpdateAction::Load,
        std::vector<std::string>{path},
        std::span<const asset::NormalMapWithId>{&m_normalMapData.back(), 1}
                    });

    return true;
}

bool NormalMapAssetManager::Load(std::span<const std::string> paths, bool isExternal)
{
    const auto newItemsIndex = m_normalMapData.size();
    const auto newNormalMapCount = static_cast<uint32_t>(paths.size());
    auto nextNormalMapIndex = static_cast<uint32_t>(newItemsIndex);
    if (newNormalMapCount + nextNormalMapIndex >= m_maxNormalMapCount)
    {
        throw NcError("Cannot exceed max normal map count.");
    }

    auto idsToLoad = std::vector<std::string>{};
    idsToLoad.reserve(paths.size());

    for (const auto& path : paths)
    {
        if (IsLoaded(path))
        {
            continue;
        }

        const auto fullPath = isExternal ? path : m_assetDirectory + path;

        m_normalMapData.emplace_back(asset::ImportTexture(fullPath), path);
        idsToLoad.push_back(path);
    }

    if (!idsToLoad.empty())
    {
        m_onUpdate.Emit(asset::NormalMapUpdateEventData{
            asset::UpdateAction::Load,
            std::move(idsToLoad),
            std::span<const asset::NormalMapWithId>{m_normalMapData.begin() + newItemsIndex, m_normalMapData.end()}
                        });
    }

    return true;
}

bool NormalMapAssetManager::Unload(const std::string& path)
{
    const auto pos = std::ranges::find_if(m_normalMapData, [&path](const auto& data)
                                          {
                                              return data.id == path;
                                          });

    if (pos == m_normalMapData.cend())
    {
        return false;
    }

    m_normalMapData.erase(pos);
    m_onUpdate.Emit(asset::NormalMapUpdateEventData{
        asset::UpdateAction::Unload,
        std::vector<std::string>{path},
        std::span<const asset::NormalMapWithId>{}
                    });

    return true;
}

void NormalMapAssetManager::UnloadAll()
{
    m_normalMapData.clear();
    /** No need to send signal to GPU - no need to write an empty buffer to the GPU. **/
}

auto NormalMapAssetManager::Acquire(const std::string& path) const -> NormalMapView
{
    const auto pos = std::ranges::find_if(m_normalMapData, [&path](const auto& data)
                                          {
                                              return data.id == path;
                                          });

    if (pos == m_normalMapData.cend())
    {
        throw NcError("Asset is not loaded: " + path);
    }

    auto index = static_cast<uint32_t>(std::distance(m_normalMapData.cbegin(), pos));
    return NormalMapView{ index };
}

bool NormalMapAssetManager::IsLoaded(const std::string& path) const
{
    auto pos = std::ranges::find_if(m_normalMapData, [&path](const auto& data)
                                    {
                                        return data.id == path;
                                    });

    return (pos != m_normalMapData.end());
}

auto NormalMapAssetManager::OnUpdate() -> Signal<const asset::NormalMapUpdateEventData&>&
{
    return m_onUpdate;
}
} // namespace nc