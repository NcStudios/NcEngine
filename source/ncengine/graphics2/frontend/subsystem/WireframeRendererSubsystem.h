#pragma once

#include "WireframeRendererState.h"

#include "ncengine/ecs/EcsFwd.h"

namespace nc
{
class Camera;
class StaticMesh;
class Transform;
class RigidBody;
struct WireframeRenderer;

namespace graphics
{
class WireframeRendererSubsystem
{
    public:
        auto BuildState(ecs::ExplicitEcs<Transform,
                                         WireframeRenderer,
                                         StaticMesh,
                                         RigidBody,
                                         Camera> worldView) -> WireframeRendererRenderState;
};
} // namespace graphics
} // namespace nc
