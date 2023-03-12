#pragma once

#include "shader_resource\EnvironmentData.h"
#include "utility/Signal.h"

namespace nc::graphics
{
struct EnvironmentFrontendState
{
    bool useSkybox;
};

class Environment
{
    public:
        Environment(Signal<const EnvironmentData&>&& backendChannel);

        void SetSkybox(const std::string& path);
        void Clear();

        auto Execute(const Vector3& cameraPosition) -> EnvironmentFrontendState;

    private:
        Signal<const EnvironmentData&> m_backendChannel;
        EnvironmentData m_environmentData;
        bool m_useSkybox;
};
} // namespace nc::graphics