#pragma once

#include "shader_resource\EnvironmentData.h"

namespace nc::graphics
{
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