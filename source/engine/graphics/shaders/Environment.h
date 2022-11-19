#pragma once

#include "math/Vector.h"

#include <string>

namespace nc::graphics
{
const std::string SkyboxMeshPath = "skybox.nca";

struct EnvironmentData
{
    Vector3 cameraWorldPosition;
    uint32_t skyboxTextureIndex;
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