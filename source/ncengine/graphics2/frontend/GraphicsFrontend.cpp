#include "GraphicsFrontend.h"
#include "FrontendRenderState.h"
#include "ncengine/ecs/Ecs.h"

namespace nc::graphics
{
auto GraphicsFrontend::BuildRenderState(ecs::Ecs world) -> FrontendRenderState
{
    auto cameraState = m_cameraSystem.BuildState(world);
    return FrontendRenderState{
        .cameraState = cameraState,
        .environmentRenderState = m_environmentSystem.BuildState(),
        .meshRenderState = m_meshSystem.BuildState(world),
        .animationRenderState = m_animationSystem.BuildState(),
        .materialRenderState = m_materialRegistry.BuildState(),
        .particleRenderState = m_particleSystem.BuildState(),
        .lightRenderState = m_lightSubsystem.BuildState(world, cameraState),
        .postProcessState = m_postProcessSystem.BuildState(),
        .wireframeRenderState = m_wireframeSystem.BuildState(world)
    };
}
} // namespace nc::graphics
