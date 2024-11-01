#include "ObjectSubsystem.h"

#include <ranges>

namespace nc::graphics
{
ObjectSubsystem::ObjectSubsystem()
    : m_sortByStatic{false}
{
}

ObjectSubsystem::ObjectSubsystem(nc::Signal<nc::graphics::ToonRenderer&>& onAddStaticRenderer,
                                 nc::Signal<nc::Entity>& onRemoveStaticRenderer)
    : m_onAddStaticRenderer{std::make_unique<Connection>(onAddStaticRenderer.Connect(this, &ObjectSubsystem::OnAddStaticRenderer))},
      m_onRemoveStaticRenderer{std::make_unique<Connection>(onRemoveStaticRenderer.Connect(this, &ObjectSubsystem::OnRemoveStaticRenderer))},
      m_staticRendererStateCache{},
      m_staticEntityCache{},
      m_isStaticRenderersDirty{false},
      m_sortByStatic{true}
{
}

auto ObjectSubsystem::BuildState(ecs::ExplicitEcs<ToonRenderer, Transform> ecs) -> ModelMatricesAndEntities
{
    const auto& rendererPool = ecs.GetPool<ToonRenderer>();
    auto entities = rendererPool.GetEntityPool();
    auto objectCount = rendererPool.size();

    auto objectRenderState = ObjectRenderState{};
    objectRenderState.modelMatrices.reserve(objectCount);

    auto sortedEntities = std::vector<Entity>{};
    sortedEntities.reserve(objectCount);

     /** We are not sorting by static so rebuild the whole list every time. Don't cache static. */
    if (!m_sortByStatic)
    {
        for (const auto& entity : entities)
        {
            objectRenderState.modelMatrices.push_back(ecs.Get<Transform>(entity).TransformationMatrix());
        }
        sortedEntities.assign(entities.begin(), entities.end()); /** I really wish we didn't have to pay this cost here in the non-sorting case, but can't figure out how not to as we can't return a span because of building up the entities list below. */
        return std::make_pair<ObjectRenderState, std::vector<Entity>>(std::move(objectRenderState), std::move(sortedEntities));
    }

    /** We are sorting by static and a static renderer has been added or removed, so rebuild the whole list. Cache the static entities. */
    if (m_isStaticRenderersDirty)
    {
        m_staticEntityCache.clear();
        m_staticEntityCache.reserve(objectCount);
        m_staticRendererStateCache.modelMatrices.clear();
        m_staticRendererStateCache.modelMatrices.reserve(objectCount);

        for (const auto& entity : entities)
        {
            if (entity.IsStatic())
            {
                m_staticEntityCache.push_back(entity);
                m_staticRendererStateCache.modelMatrices.push_back(ecs.Get<Transform>(entity).TransformationMatrix());
                continue;
            }

            sortedEntities.push_back(entity);
            objectRenderState.modelMatrices.push_back(ecs.Get<Transform>(entity).TransformationMatrix());
        }

        objectRenderState.modelMatrices.insert_range(objectRenderState.modelMatrices.begin(), m_staticRendererStateCache.modelMatrices);
        sortedEntities.insert_range(sortedEntities.begin(), m_staticEntityCache);
        return std::make_pair<ObjectRenderState, std::vector<Entity>>(std::move(objectRenderState), std::move(sortedEntities));
    }

    /** We are sorting by static and static renderers have not changed. Just rebuild the non-static list. */
    for (const auto& entity : entities)
    {
        if (!entity.IsStatic())
        {
            sortedEntities.push_back(entity);
            objectRenderState.modelMatrices.push_back(ecs.Get<Transform>(entity).TransformationMatrix());
        }
    }

    objectRenderState.modelMatrices.insert_range(objectRenderState.modelMatrices.begin(), m_staticRendererStateCache.modelMatrices);
    sortedEntities.insert_range(sortedEntities.begin(), m_staticEntityCache);
    return std::make_pair<ObjectRenderState, std::vector<Entity>>(std::move(objectRenderState), std::move(sortedEntities));
}
} // namespace nc::graphics
