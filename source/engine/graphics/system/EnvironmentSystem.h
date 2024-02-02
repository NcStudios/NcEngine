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
        EnvironmentSystem(Signal<const GlobalData&>&& backendChannel);

        void SetSkybox(const std::string& path);
        void Clear();

        auto Execute(const CameraState& cameraState) -> EnvironmentState;

    private:
        Signal<const GlobalData&> m_backendChannel;
        GlobalData m_environmentData;
        bool m_useSkybox;
};
} // namespace nc::graphics