#include "MeshRendererSubsystem.h"
#include "ncengine/ecs/Ecs.h"
#include "ncengine/ecs/Transform.h"
#include "ncengine/graphics/MeshRenderer2.h"

#include <ranges>

namespace nc::graphics
{
auto MeshRendererSubsystem::BuildState(ecs::ExplicitEcs<MeshRenderer2, Transform> ecs) -> MeshRendererRenderState
{
    const auto& rendererPool = ecs.GetPool<MeshRenderer2>();
    const auto entities = rendererPool.GetEntityPool();
    m_rendererDataCache.clear();
    m_rendererDataCache.reserve(entities.size());

    std::ranges::transform(entities, std::back_inserter(m_rendererDataCache), [&ecs](Entity entity)
    {
        return MeshRendererData{
            ecs.Get<Transform>(entity).TransformationMatrix(),
            ecs.Get<MeshRenderer2>(entity).GetMaterial().GetHandle()
        };
    });

    return MeshRendererRenderState{.modelMatrices = m_rendererDataCache, .entities = entities};
}
} // namespace nc::graphics
