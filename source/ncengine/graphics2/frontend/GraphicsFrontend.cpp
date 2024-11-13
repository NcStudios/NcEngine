#include "GraphicsFrontend.h"
#include "FrontendRenderState.h"
#include "ncengine/ecs/Ecs.h"

namespace nc::graphics
{
auto GraphicsFrontend::BuildRenderState(ecs::Ecs world) -> FrontendRenderState
{
    return FrontendRenderState{
        .cameraState = m_cameraSystem.BuildState(world),
        .meshRendererState = m_meshRendererSystem.BuildState(world),
        .materialRenderState = m_materialRegistry.BuildState()
    };
}
} // namespace nc::graphics
