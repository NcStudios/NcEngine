#pragma once

#include "asset/Assets.h"
#include "ecs/View.h"
#include "ecs/Transform.h"
#include "graphics/MeshRenderer.h"
#include "utility/Signal.h"

#include <vector>

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

class ObjectSystem
{
    public:
        ObjectSystem(Signal<const std::vector<ObjectData>&>&& backendPort)
            : m_backendPort{std::move(backendPort)}
        {
        }

        auto Execute(MultiView<MeshRenderer, Transform> gameState,
                     const CameraFrontendState& cameraState,
                     EnvironmentFrontendState& environmentState) -> ObjectFrontendState;

    private:
        Signal<const std::vector<ObjectData>&> m_backendPort;
};
} // namespace nc::graphics
