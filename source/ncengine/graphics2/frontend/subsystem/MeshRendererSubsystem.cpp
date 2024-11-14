#include "MeshRendererSubsystem.h"
#include "ncengine/ecs/Ecs.h"
#include "ncengine/ecs/Transform.h"
#include "ncengine/graphics/MeshRenderer2.h"

#include <ranges>

namespace nc::graphics
{
MeshRendererSubsystem::MeshRendererSubsystem(std::span<const MaterialPass::type> passes)
    : m_instanceCache{100000}, // todo: pass in
      m_passCache{passes},
      m_ctx{m_instanceCache, m_passCache}
{
    MeshRenderer2::SetContext(&m_ctx);
}

auto MeshRendererSubsystem::BuildState(ecs::ExplicitEcs<MeshRenderer2, Transform> ecs) -> MeshRendererRenderState
{
    m_passCache.ClearDynamicTargets();

    m_instanceCache.PopulateMatrices(ecs);

    return MeshRendererRenderState{
        .instanceData = m_instanceCache.BuildState(),
        .passData = m_passCache.BuildState()
    };
}
} // namespace nc::graphics
