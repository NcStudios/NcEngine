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
        .materialRenderState = m_materialRegistry.BuildState(),
        .lightRenderState = m_lightSubsystem.BuildState(world, world, world) /** This can't be the right usage of ExplicitEcs */
    };
}
} // namespace nc::graphics
