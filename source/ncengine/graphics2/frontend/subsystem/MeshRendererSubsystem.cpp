#include "MeshRendererSubsystem.h"

#include <ranges>

namespace nc::graphics
{
auto MeshRendererSubsystem::BuildState(ecs::ExplicitEcs<ToonRenderer, Transform> ecs) -> MeshRendererRenderState
{
    const auto& rendererPool = ecs.GetPool<ToonRenderer>();
    auto entities = rendererPool.GetEntityPool();
    m_modelMatricesCache.clear();
    m_modelMatricesCache.reserve(entities.size());

    std::ranges::transform(entities, std::back_inserter(m_modelMatricesCache), [&ecs](Entity entity) 
    {
        return MeshRendererData{ecs.Get<Transform>(entity).TransformationMatrix()};
    });

    return MeshRendererRenderState{.modelMatrices = m_modelMatricesCache, .entities = entities};
}
} // namespace nc::graphics
