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
        explicit CubeMapAssetManager(const std::string& cubeMapAssetDirectory, uint32_t maxCubeMapsCount);

        bool Load(const std::string& path, AssetSubtype = AssetSubtype::None) override;
        bool Load(std::span<const std::string> paths, AssetSubtype = AssetSubtype::None) override;
        bool Unload(const std::string& path) override;
        void UnloadAll() override;
        auto Acquire(const std::string& path) const -> CubeMapView override;
        auto Acquire(AssetId id) const -> CubeMapView override;
        bool IsLoaded(const std::string& path) const override;
        auto GetPath(AssetId id) const -> std::string_view override { return m_cubeMapIds.at(m_cubeMapIds.index(id)); }
        auto GetAllLoaded() const -> std::vector<std::string_view> override;
        auto GetAssetType() const noexcept -> asset::AssetType override { return asset::AssetType::CubeMap; }
        auto OnUpdate() -> Signal<const asset::CubeMapUpdateEventData&>&;

    private:
        StringTable m_cubeMapIds;
        std::string m_assetDirectory;
        uint32_t m_maxCubeMapsCount;
        Signal<const asset::CubeMapUpdateEventData&> m_onUpdate;
};
} // namesapce nc::asset

