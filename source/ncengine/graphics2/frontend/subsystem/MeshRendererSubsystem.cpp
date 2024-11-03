#include "MeshRendererSubsystem.h"
#include "ncengine/Events.h"

#include <ranges>

namespace
{
void ClearAndReserve(std::vector<DirectX::XMMATRIX>& modelMatrices, std::vector<nc::Entity>& entities, size_t capacity)
{
    modelMatrices.clear();
    modelMatrices.reserve(capacity);

    entities.clear();
    entities.reserve(capacity);
}
} // anonymous namespace

namespace nc::graphics
{
MeshRendererSubsystem::MeshRendererSubsystem()
    : m_sortByStatic{false}
{
}

MeshRendererSubsystem::MeshRendererSubsystem(nc::Signal<nc::graphics::ToonRenderer&>& onAddRenderer,
                                             nc::Signal<nc::Entity>& onRemoveRenderer,
                                             SystemEvents& events)
    : m_onAddRenderer{std::make_unique<Connection>(onAddRenderer.Connect(this, &MeshRendererSubsystem::OnAddRenderer))},
      m_onRemoveRenderer{std::make_unique<Connection>(onRemoveRenderer.Connect(this, &MeshRendererSubsystem::OnRemoveRenderer))},
      m_onStaticEntitiesRebuilt{std::make_unique<Connection>(events.rebuildStatics.Connect(this, &MeshRendererSubsystem::OnStaticEntitiesRebuilt))},
      m_staticMatricesCache{},
      m_staticEntityCache{},
      m_dynamicMatricesCache{},
      m_dynamicEntityCache{},
      m_isStaticRenderersDirty{false},
      m_sortByStatic{true}
{
}

auto MeshRendererSubsystem::BuildState(ecs::ExplicitEcs<ToonRenderer, Transform> ecs) -> MeshRendererRenderState
{
    const auto& rendererPool = ecs.GetPool<ToonRenderer>();
    auto entities = rendererPool.GetEntityPool();
    auto objectCount = rendererPool.size();

     /** We are not sorting by static so rebuild the whole list every time. Don't cache static. */
    if (!m_sortByStatic)
    {
        ClearAndReserve(m_dynamicMatricesCache, m_dynamicEntityCache, objectCount);

        std::ranges::transform(entities, std::back_inserter(m_dynamicMatricesCache), [&ecs](Entity entity) 
        {
            return ecs.Get<Transform>(entity).TransformationMatrix();
        });

        m_dynamicEntityCache.assign(entities.begin(), entities.end());
        return MeshRendererRenderState{.modelMatrices = m_dynamicMatricesCache, .entities = m_dynamicEntityCache};
    }

    /** We are sorting by static and a static renderer has been added or removed, so rebuild the whole list. Cache the static entities. */
    if (m_isStaticRenderersDirty || m_staticEntityCache.empty())
    {
        ClearAndReserve(m_staticMatricesCache, m_staticEntityCache, objectCount);
        ClearAndReserve(m_dynamicMatricesCache, m_dynamicEntityCache, objectCount);

        for (const auto& entity : entities)
        {
            if (entity.IsStatic())
            {
                m_staticEntityCache.push_back(entity);
                m_staticMatricesCache.push_back(ecs.Get<Transform>(entity).TransformationMatrix());
                continue;
            }

            m_dynamicEntityCache.push_back(entity);
            m_dynamicMatricesCache.push_back(ecs.Get<Transform>(entity).TransformationMatrix());
        }

        /** We need to return the combined lists of both static and dynamic here. We can add static to m_dynamicEntityCache/RenderStateCache because we are always going to blow away 
         * m_dynamicEntityCache before readding to it, so it can act as our temporary vector here.*/
        m_dynamicMatricesCache.insert_range(m_dynamicMatricesCache.begin(), m_staticMatricesCache);
        m_dynamicEntityCache.insert_range(m_dynamicEntityCache.begin(), m_staticEntityCache);

        m_isStaticRenderersDirty = false;
        return MeshRendererRenderState{.modelMatrices = m_dynamicMatricesCache, .entities = m_dynamicEntityCache};
    }

    /** We are sorting by static and static renderers have not changed. Just rebuild the non-static list. */
    ClearAndReserve(m_dynamicMatricesCache, m_dynamicEntityCache, objectCount);

    for (const auto& entity : entities)
    {
        if (!entity.IsStatic())
        {
            m_dynamicEntityCache.push_back(entity);
            m_dynamicMatricesCache.push_back(ecs.Get<Transform>(entity).TransformationMatrix());
        }
    }

    m_dynamicMatricesCache.insert_range(m_dynamicMatricesCache.begin(), m_staticMatricesCache);
    m_dynamicEntityCache.insert_range(m_dynamicEntityCache.begin(), m_staticEntityCache);
    return MeshRendererRenderState{.modelMatrices = m_dynamicMatricesCache, .entities = m_dynamicEntityCache};
}
} // namespace nc::graphics
