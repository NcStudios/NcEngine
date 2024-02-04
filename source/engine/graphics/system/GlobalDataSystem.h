#pragma once

#include "graphics/shader_resource/GlobalData.h"
#include "utility/Signal.h"

namespace nc::graphics
{
struct CameraState;

struct GlobalDataState
{
    bool useSkybox;
};

class GlobalDataSystem
{
    public:
        GlobalDataSystem(Signal<const GlobalData&>&& backendChannel);

        void SetSkybox(const std::string& path);
        void Clear();

        auto Execute(const CameraState& cameraState) -> GlobalDataState;

    private:
        Signal<const GlobalData&> m_backendChannel;
        GlobalData m_globalData;
        bool m_useSkybox;
};
} // namespace nc::graphics
