#include "ObjectSubsystem.h"
#include "ncengine/Events.h"

#include <ranges>

namespace nc::graphics
{
ObjectSubsystem::ObjectSubsystem()
    : m_sortByStatic{false}
{
}

ObjectSubsystem::ObjectSubsystem(nc::Signal<nc::graphics::ToonRenderer&>& onAddRenderer,
                                 nc::Signal<nc::Entity>& onRemoveRenderer,
                                 SystemEvents& events)
    : m_onAddRenderer{std::make_unique<Connection>(onAddRenderer.Connect(this, &ObjectSubsystem::OnAddRenderer))},
      m_onRemoveRenderer{std::make_unique<Connection>(onRemoveRenderer.Connect(this, &ObjectSubsystem::OnRemoveRenderer))},
      m_onStaticEntitiesRebuilt{std::make_unique<Connection>(events.rebuildStatics.Connect(this, &ObjectSubsystem::OnStaticEntitiesRebuilt))},
      m_staticRendererStateCache{},
      m_staticEntityCache{},
      m_dynamicRendererStateCache{},
      m_dynamicEntityCache{},
      m_isStaticRenderersDirty{false},
      m_sortByStatic{true}
{
}

auto ObjectSubsystem::BuildState(ecs::ExplicitEcs<ToonRenderer, Transform> ecs) -> ModelMatricesAndEntities
{
    const auto& rendererPool = ecs.GetPool<ToonRenderer>();
    auto entities = rendererPool.GetEntityPool();
    auto objectCount = rendererPool.size();

     /** We are not sorting by static so rebuild the whole list every time. Don't cache static. */
    if (!m_sortByStatic)
    {
        m_dynamicEntityCache.clear();
        m_dynamicEntityCache.reserve(objectCount);
        m_dynamicRendererStateCache.modelMatrices.clear();
        m_dynamicRendererStateCache.modelMatrices.reserve(objectCount);

        std::ranges::transform(entities, std::back_inserter(m_dynamicRendererStateCache.modelMatrices), [&ecs](Entity entity) 
        {
            return ecs.Get<Transform>(entity).TransformationMatrix();
        });
        m_dynamicEntityCache.assign(entities.begin(), entities.end());
        return std::make_pair<ObjectRenderState, std::span<const Entity>>(std::move(m_dynamicRendererStateCache), m_dynamicEntityCache);
    }

    /** We are sorting by static and a static renderer has been added or removed, so rebuild the whole list. Cache the static entities. */
    if (m_isStaticRenderersDirty || m_staticEntityCache.empty())
    {
        m_staticEntityCache.clear();
        m_staticEntityCache.reserve(objectCount);
        m_staticRendererStateCache.modelMatrices.clear();
        m_staticRendererStateCache.modelMatrices.reserve(objectCount);

        m_dynamicEntityCache.clear();
        m_dynamicEntityCache.reserve(objectCount);
        m_dynamicRendererStateCache.modelMatrices.clear();
        m_dynamicRendererStateCache.modelMatrices.reserve(objectCount);

        for (const auto& entity : entities)
        {
            if (entity.IsStatic())
            {
                m_staticEntityCache.push_back(entity);
                m_staticRendererStateCache.modelMatrices.push_back(ecs.Get<Transform>(entity).TransformationMatrix());
                continue;
            }

            m_dynamicEntityCache.push_back(entity);
            m_dynamicRendererStateCache.modelMatrices.push_back(ecs.Get<Transform>(entity).TransformationMatrix());
        }

        /** We need to return the combined lists of both static and dynamic here. We can add static to m_dynamicEntityCache/RendererStateCache because we are always going to blow away 
         * m_dynamicEntityCache before readding to it, so it can act as our temporary vector here.*/
        m_dynamicRendererStateCache.modelMatrices.insert_range(m_dynamicRendererStateCache.modelMatrices.begin(), m_staticRendererStateCache.modelMatrices);
        m_dynamicEntityCache.insert_range(m_dynamicEntityCache.begin(), m_staticEntityCache);

        m_isStaticRenderersDirty = false;
        return std::make_pair<ObjectRenderState, std::span<const Entity>>(std::move(m_dynamicRendererStateCache), m_dynamicEntityCache);
    }

    /** We are sorting by static and static renderers have not changed. Just rebuild the non-static list. */
    m_dynamicEntityCache.clear();
    m_dynamicEntityCache.reserve(objectCount);
    m_dynamicRendererStateCache.modelMatrices.clear();
    m_dynamicRendererStateCache.modelMatrices.reserve(objectCount);

    for (const auto& entity : entities)
    {
        if (!entity.IsStatic())
        {
            m_dynamicEntityCache.push_back(entity);
            m_dynamicRendererStateCache.modelMatrices.push_back(ecs.Get<Transform>(entity).TransformationMatrix());
        }
    }

    m_dynamicRendererStateCache.modelMatrices.insert_range(m_dynamicRendererStateCache.modelMatrices.begin(), m_staticRendererStateCache.modelMatrices);
    m_dynamicEntityCache.insert_range(m_dynamicEntityCache.begin(), m_staticEntityCache);
    return std::make_pair<ObjectRenderState, std::span<const Entity>>(std::move(m_dynamicRendererStateCache), m_dynamicEntityCache);
}
} // namespace nc::graphics
