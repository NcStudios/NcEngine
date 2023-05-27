#pragma once

#include "assets/AssetService.h"
#include "utility/Signal.h"

#include <string>
#include <vector>

namespace nc
{
namespace asset
{
struct NormalMapWithId;
struct NormalMapUpdateEventData;
} // namespace nc

class NormalMapAssetManager : public IAssetService<NormalMapView, std::string>
{
    public:
    explicit NormalMapAssetManager(const std::string& normalMapsAssetDirectory, uint32_t maxNormalMaps);

    bool Load(const std::string& path, bool isExternal) override;
    bool Load(std::span<const std::string> paths, bool isExternal) override;
    bool Unload(const std::string& path) override;
    void UnloadAll() override;
    auto Acquire(const std::string& path) const->NormalMapView override;
    bool IsLoaded(const std::string& path) const override;
    auto OnUpdate() -> Signal<const asset::NormalMapUpdateEventData&>&;

    private:
    std::vector<asset::NormalMapWithId> m_normalMapData;
    std::string m_assetDirectory;
    uint32_t m_maxNormalMapCount;
    Signal<const asset::NormalMapUpdateEventData&> m_onUpdate;
};
} // namespace nc
