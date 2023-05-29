#pragma once

#include "assets/AssetService.h"
#include "utility/Signal.h"

#include <string>
#include <unordered_map>
#include <vector>

namespace nc
{
namespace asset
{
struct CubeMapUpdateEventData;
} // namespace asset

class CubeMapAssetManager : public IAssetService<CubeMapView, std::string>
{
    public:
        explicit CubeMapAssetManager(const std::string& cubeMapAssetDirectory, uint32_t maxCubeMapsCount);

        bool Load(const std::string& path, bool isExternal, asset_flags_type flags = AssetFlags::None) override;
        bool Load(std::span<const std::string> paths, bool isExternal, asset_flags_type flags = AssetFlags::None) override;
        bool Unload(const std::string& path, asset_flags_type flags = AssetFlags::None) override;
        void UnloadAll(asset_flags_type flags = AssetFlags::None) override;
        auto Acquire(const std::string& path, asset_flags_type flags = AssetFlags::None) const -> CubeMapView override;
        bool IsLoaded(const std::string& path, asset_flags_type flags = AssetFlags::None) const override;
        auto OnUpdate() -> Signal<const asset::CubeMapUpdateEventData&>&;

    private:
        std::vector<std::string> m_cubeMapIds;
        std::string m_assetDirectory;
        uint32_t m_maxCubeMapsCount;
        Signal<const asset::CubeMapUpdateEventData&> m_onUpdate;
};
} // namesapce nc

