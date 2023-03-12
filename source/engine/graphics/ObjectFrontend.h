#pragma once

#include "asset/Assets.h"
#include "utility/Signal.h"

#include <vector>

namespace nc { class Registry; struct MeshView; }

namespace nc::graphics
{
struct CameraFrontendState;
struct EnvironmentFrontendState;
struct ObjectData;

struct ObjectFrontendState
{
    std::vector<MeshView> meshes;
    // optional?
    uint32_t skyboxInstanceIndex;
};

class ObjectFrontend
{
    public:
        ObjectFrontend(Signal<const std::vector<ObjectData>&>&& backendPort)
            : m_backendPort{std::move(backendPort)}
        {
        }

        auto Execute(Registry* registry,
                     const CameraFrontendState& cameraState,
                     EnvironmentFrontendState& environmentState) -> ObjectFrontendState;

        void Clear() noexcept;

    private:
        Signal<const std::vector<ObjectData>&> m_backendPort;
};
} // namespace nc::graphics
