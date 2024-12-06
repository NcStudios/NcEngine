#pragma once

#include "WireframeRendererState.h"

#include "ncengine/ecs/EcsFwd.h"

namespace nc
{
class MeshRenderer2;
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
                                         MeshRenderer2,
                                         RigidBody> worldView) -> WireframeRendererRenderState;
};
} // namespace graphics
} // namespace nc
