#pragma once

#include "ncmath/Vector.h"

#include <string>

namespace nc::graphics
{
const std::string SkyboxMeshPath = "skybox.nca";

struct EnvironmentData
{
    Vector3 cameraWorldPosition = Vector3(0.0f, 0.0f, 0.0f);
    uint32_t skyboxTextureIndex = 0u;
};

class Environment
{
    public:
        Environment();

        void SetSkybox(const std::string& path);
        void SetCameraPosition(const Vector3& cameraPosition);
        auto Get() const -> const EnvironmentData&;
        void Clear();
        bool UseSkybox();

    private:
        EnvironmentData m_environmentData;
        bool m_useSkybox;
};
} // namespace nc::graphics