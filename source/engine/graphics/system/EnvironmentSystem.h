#pragma once

#include "graphics/shader_resource/EnvironmentData.h"
#include "utility/Signal.h"

namespace nc::graphics
{
struct CameraFrontendState;

struct EnvironmentFrontendState
{
    bool useSkybox;
};

class EnvironmentSystem
{
    public:
        EnvironmentSystem(Signal<const EnvironmentData&>&& backendChannel);

        void SetSkybox(const std::string& path);
        void Clear();

        auto Execute(const CameraFrontendState& cameraState) -> EnvironmentFrontendState;

    private:
        Signal<const EnvironmentData&> m_backendChannel;
        EnvironmentData m_environmentData;
        bool m_useSkybox;
};
} // namespace nc::graphics