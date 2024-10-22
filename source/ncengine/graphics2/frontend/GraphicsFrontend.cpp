#include "GraphicsFrontend.h"
#include "FrontendRenderState.h"
#include "ncengine/ecs/Ecs.h"

namespace nc::graphics
{
auto GraphicsFrontend::BuildRenderState(ecs::Ecs world) -> FrontendRenderState
{
    return FrontendRenderState{
        .cameraState = camera.BuildState(world)
    };
}
} // namespace nc::graphics
