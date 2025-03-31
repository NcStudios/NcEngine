#pragma once

#include "EnvironmentRenderState.h"
#include "asset/AssetService.h"

#include <cstdint>

namespace nc::graphics
{
class EnvironmentSubsystem
{
    public:
        void SetSkybox(const std::string& skyboxPath)
        {
            m_useSkybox = true;
            m_skyboxIndex = asset::AssetService<asset::CubeMapView>::Get()->Acquire(skyboxPath).index;
            m_skyboxId = asset::AssetService<asset::CubeMapView>::Get()->Acquire(skyboxPath).id;
        };

        void ClearSkybox()
        {
            m_useSkybox = false;
            m_skyboxIndex = std::numeric_limits<uint32_t>::max();
            m_skyboxId = nc::asset::NullAssetId;
        };

        auto GetSkybox() const -> nc::asset::AssetId { return m_skyboxId; }
        auto BuildState() -> EnvironmentRenderState { return EnvironmentRenderState{.skyboxIndex = m_skyboxIndex, .useSkybox = static_cast<uint32_t>(m_useSkybox)};};

    private:
        uint32_t m_skyboxIndex = std::numeric_limits<uint32_t>::max();
        nc::asset::AssetId m_skyboxId = nc::asset::NullAssetId;
        bool m_useSkybox = false;
};
}