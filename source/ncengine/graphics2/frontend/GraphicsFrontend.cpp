#include "GraphicsFrontend.h"
#include "FrontendRenderState.h"
#include "ncengine/ecs/Ecs.h"

namespace nc::graphics
{
auto GraphicsFrontend::BuildRenderState(ecs::Ecs world) -> FrontendRenderState
{
    return FrontendRenderState{
        .cameraState = m_cameraSystem.BuildState(world),
        .meshRenderState = m_meshSystem.BuildState(world),
        .animationRenderState = m_animationSystem.BuildState(),
        .materialRenderState = m_materialRegistry.BuildState(),
        .lightRenderState = m_lightSubsystem.BuildState(world),
        .postProcessState = m_postProcessSystem.BuildState(),
        .wireframeRenderState = m_wireframeSystem.BuildState(world)
    };
}
} // namespace nc::graphics
