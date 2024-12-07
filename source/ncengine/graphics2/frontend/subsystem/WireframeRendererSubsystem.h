#pragma once

#include "WireframeRendererState.h"

#include "ncengine/ecs/EcsFwd.h"

namespace nc
{
class StaticMesh;
class Transform;
class RigidBody;

namespace graphics
{
struct WireframeRenderer;

class WireframeRendererSubsystem
{
    public:
        auto BuildState(ecs::ExplicitEcs<Transform,
                                         WireframeRenderer,
                                         StaticMesh,
                                         RigidBody> worldView) -> WireframeRendererRenderState;
};
} // namespace graphics
} // namespace nc
