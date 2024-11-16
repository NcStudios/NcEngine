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
    m_instanceData.clear();
    m_instanceData.reserve(entities.size());
    m_passCache.ClearDynamicTargets();

    for (auto [i, entity] : std::views::enumerate(entities))
    {
        /** @todo 798 filter static */
        auto& renderer = ecs.Get<MeshRenderer2>(entity);
        const auto& material = renderer.GetMaterial();
        m_passCache.AddDynamicTarget(
            material.GetPasses(),
            static_cast<uint32_t>(i),
            renderer.GetMesh()
        );

        m_instanceData.emplace_back(
            ecs.Get<Transform>(entity).TransformationMatrix(),
            material.GetHandle()
        );
    }

    return MeshRendererRenderState{
        .instanceData = m_instanceData,
        .passData = m_passCache.BuildState()
    };
}
} // namespace nc::graphics
