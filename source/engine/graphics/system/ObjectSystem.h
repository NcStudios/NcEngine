#pragma once

#include "asset/Assets.h"
#include "ecs/View.h"
#include "ecs/Transform.h"
#include "graphics/MeshRenderer.h"
#include "graphics/ToonRenderer.h"
#include "utility/Signal.h"

#include <optional>
#include <vector>

namespace nc::graphics
{
struct CameraState;
struct EnvironmentState;
struct ObjectData;
struct SkeletalAnimationSystemState;

struct ObjectState
{
    std::vector<MeshView> pbrMeshes;
    uint32_t pbrMeshStartingIndex;
    std::vector<MeshView> toonMeshes;
    uint32_t toonMeshStartingIndex;
    std::optional<uint32_t> skyboxInstanceIndex = std::nullopt;
};

class ObjectSystem
{
    public:
        ObjectSystem(Signal<const std::vector<ObjectData>&>&& backendPort)
            : m_backendPort{std::move(backendPort)}
        {
        }

        auto Execute(MultiView<MeshRenderer, Transform> pbrRenderers,
                     MultiView<ToonRenderer, Transform> toonRenderers,
                     const CameraState& cameraState,
                     const EnvironmentState& environmentState,
                     const SkeletalAnimationSystemState& skeletalAnimationState) -> ObjectState;

    private:
        Signal<const std::vector<ObjectData>&> m_backendPort;
};
} // namespace nc::graphics
