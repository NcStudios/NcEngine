#pragma once

#include "EnvironmentRenderState.h"
#include "ncengine/graphics/Environment.h"
#include "asset/AssetService.h"

#include <cstdint>

namespace nc::graphics
{
class EnvironmentSubsystem
{
    public:
        auto GetSkybox() const -> nc::asset::AssetId { return m_environment.skybox.id; }

        void SetSkybox(const std::string& skyboxPath)
        {
            m_environment.skybox = asset::AssetService<asset::CubeMapView>::Get()->Acquire(skyboxPath);
            m_environment.useSkybox = static_cast<uint32_t>(m_environment.skybox.id != nc::asset::NullAssetId);
        };

        void ClearSkybox()
        {
            m_environment.skybox = asset::CubeMapView{};
            m_environment.useSkybox = false;
        };

        auto GetEnvironment() const -> const nc::Environment& { return m_environment; }

        void SetEnvironment(const Environment& environment)
        {
            m_environment = environment;
        };

        void ClearEnvironment() 
        {
            m_environment = Environment{};
        }

        auto BuildState() -> EnvironmentRenderState
        { 
            return EnvironmentRenderState{
                .primaryColor = m_environment.primaryColor,
                .secondaryColor = m_environment.secondaryColor,
                .tertiaryColor = m_environment.tertiaryColor,
                .skyboxIndex = m_environment.skybox.index,
                .useSkybox = m_environment.useSkybox,
                .useColorOverride = m_environment.useColorOverride
            };
        };

    private:
        Environment m_environment;
};
}
