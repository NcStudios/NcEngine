#pragma once

#include "asset/AssetService.h"
#include "utility/StringMap.h"
#include "ncengine/utility/Signal.h"

#include <string>

namespace nc::asset
{
struct CubeMapUpdateEventData;

class CubeMapAssetManager : public IAssetService<CubeMapView, std::string>
{
    public:
        explicit CubeMapAssetManager(const std::string& cubeMapAssetDirectory,
                                     uint32_t maxCubeMapsCount);

        auto Load(const std::string& path)                     -> bool                          override;
        auto Load(std::span<const std::string> paths)          -> bool                          override;
        auto Unload(const std::string& path)                   -> bool                          override;
        void UnloadAll() override; 
        auto Acquire(const std::string& path)            const -> CubeMapView                   override;
        auto Acquire(AssetId id)                         const -> CubeMapView                   override;
        auto GetAllLoaded(bool serializableOnly = false) const -> std::vector<std::string_view> override;
        auto IsLoaded(const std::string& path)           const -> bool                          override { return m_cubeMapIds.contains(path); }
        auto GetPath(AssetId id)                         const -> std::string_view              override { return m_cubeMapIds.at(m_cubeMapIds.index(id)); }
        auto GetAssetType()                              const -> asset::AssetType              override { return asset::AssetType::CubeMap; }
        auto OnUpdate()                                        -> decltype(auto)                         { return (m_onUpdate); }

    private:
        StringTable m_cubeMapIds;
        std::string m_assetDirectory;
        uint32_t m_maxCubeMapsCount;
        Signal<const asset::CubeMapUpdateEventData&> m_onUpdate;
};
} // namesapce nc::asset

