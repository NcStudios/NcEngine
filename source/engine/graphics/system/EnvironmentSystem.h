#pragma once

#include "graphics/shader_resource/EnvironmentData.h"
#include "utility/Signal.h"

namespace nc::graphics
{
struct CameraState;

struct EnvironmentState
{
    bool useSkybox;
};

class EnvironmentSystem
{
    public:
        EnvironmentSystem(Signal<const EnvironmentData&>&& backendChannel);

        void SetSkybox(const std::string& path);
        void Clear();

        auto Execute(const CameraState& cameraState) -> EnvironmentState;

    private:
        Signal<const EnvironmentData&> m_backendChannel;
        EnvironmentData m_environmentData;
        bool m_useSkybox;
};
} // namespace nc::graphics