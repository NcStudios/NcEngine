#include "MeshRendererSubsystem.h"
#include "ncengine/ecs/Ecs.h"
#include "ncengine/ecs/Transform.h"
#include "ncengine/graphics/MeshRenderer2.h"
#include "ncengine/graphics/ToonRenderer.h"

#include <ranges>


#include <iostream>

namespace nc::graphics
{
auto MeshRendererSubsystem::BuildState(ecs::ExplicitEcs<MeshRenderer2, ToonRenderer, Transform> ecs) -> MeshRendererRenderState
{
    const auto& rendererPool = ecs.GetPool<MeshRenderer2>();
    const auto entities = rendererPool.GetEntityPool();
    m_rendererDataCache.clear();
    m_rendererDataCache.reserve(entities.size());

    std::ranges::transform(entities, std::back_inserter(m_rendererDataCache), [&ecs](Entity entity)
    {
        return MeshRendererData{
            ecs.Get<Transform>(entity).TransformationMatrix(),
            // 1
            ecs.Get<MeshRenderer2>(entity).GetMaterial().GetHandle()
        };
    });

    std::cout << "matIndices:\n";
    for (const auto& [m, i] : m_rendererDataCache)
    {
        std::cout << "\t" << i << '\n';
    }

    return MeshRendererRenderState{.modelMatrices = m_rendererDataCache, .entities = entities};
}
} // namespace nc::graphics
