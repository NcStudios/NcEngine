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
            m_view = asset::AssetService<asset::CubeMapView>::Get()->Acquire(skyboxPath);
            m_skyboxIndex = m_view.index;
            m_skyboxId = m_view.id;
        };

        void ClearSkybox()
        {
            m_skyboxIndex = std::numeric_limits<uint32_t>::max();
            m_skyboxId = nc::asset::NullAssetId;
        };

        auto GetSkybox() const -> nc::asset::AssetId { return m_skyboxId; }
        auto BuildState() -> EnvironmentRenderState { return EnvironmentRenderState{.skyboxIndex = m_skyboxIndex, .useSkybox = static_cast<uint32_t>(m_skyboxId != nc::asset::NullAssetId)};};

    private:
        uint32_t m_skyboxIndex = std::numeric_limits<uint32_t>::max();
        nc::asset::CubeMapView m_view;
        nc::asset::AssetId m_skyboxId = nc::asset::NullAssetId;
};
}