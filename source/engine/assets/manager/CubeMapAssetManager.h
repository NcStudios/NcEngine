#pragma once

#include "assets/AssetService.h"
#include "utility/Signal.h"

#include <string>
#include <unordered_map>
#include <vector>

namespace nc
{
struct CubeMapBufferData;

class CubeMapAssetManager : public IAssetService<CubeMapView, std::string>
{
    public:
        explicit CubeMapAssetManager(const std::string& cubeMapAssetDirectory, uint32_t maxCubeMapsCount);

        bool Load(const std::string& path, bool isExternal) override;
        bool Load(std::span<const std::string> paths, bool isExternal) override;
        bool Unload(const std::string& path) override;
        void UnloadAll() override;
        auto Acquire(const std::string& path) const -> CubeMapView override;
        bool IsLoaded(const std::string& path) const override;
        auto OnUpdate() -> Signal<const CubeMapBufferData&>*;

    private:
        std::vector<std::string> m_cubeMapIds;
        std::string m_assetDirectory;
        uint32_t m_maxCubeMapsCount;
        Signal<const CubeMapBufferData&> m_onUpdate;
};
} // namesapce nc

