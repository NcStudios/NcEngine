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
        };

        void ClearSkybox()
        {
            m_view = asset::CubeMapView{};
        };

        auto GetSkybox() const -> nc::asset::AssetId { return m_view.id; }
        auto BuildState() -> EnvironmentRenderState { return EnvironmentRenderState{.skyboxIndex = m_view.index, .useSkybox = static_cast<uint32_t>(m_view.id != nc::asset::NullAssetId)};};

    private:
        uint32_t m_skyboxIndex = std::numeric_limits<uint32_t>::max();
        nc::asset::CubeMapView m_view;
        nc::asset::AssetId m_skyboxId = nc::asset::NullAssetId;
};
}
