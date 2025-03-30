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
        };

        void ClearSkybox()
        {
            m_useSkybox = false;
        };

        auto BuildState() -> EnvironmentRenderState { return EnvironmentRenderState{.skyboxIndex = m_skyboxIndex, .useSkybox = static_cast<uint32_t>(m_useSkybox)};};

    private:
        uint32_t m_skyboxIndex = 0u;
        bool m_useSkybox = false;
};
}