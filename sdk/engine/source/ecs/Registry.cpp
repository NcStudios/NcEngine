#include "ecs/Registry.h"
#include "ecs/All.h"

/** @todo This needs to go away. */
namespace
{
    nc::Registry* g_registry = nullptr;
}

namespace nc
{
    auto ActiveRegistry() -> Registry*
    {
        return g_registry;
    }

    Registry::Registry(size_t maxEntities)
        : m_registeredStorage{},
          m_entities{},
          m_maxEntities{maxEntities}
    {
        g_registry = this;

        RegisterComponentType<CollisionLogic>();
        RegisterComponentType<FrameLogic>();
        RegisterComponentType<FixedLogic>();
        RegisterComponentType<Tag>();
        RegisterComponentType<Transform>();
        RegisterComponentType<audio::AudioSource>();
        RegisterComponentType<ecs::detail::FreeComponentGroup>();
        RegisterComponentType<graphics::MeshRenderer>();
        RegisterComponentType<graphics::ParticleEmitter>();
        RegisterComponentType<graphics::PointLight>();
        RegisterComponentType<net::NetworkDispatcher>();
        RegisterComponentType<physics::Collider>();
        RegisterComponentType<physics::ConcaveCollider>();
        RegisterComponentType<physics::PhysicsBody>();
    }

    void Registry::RemoveEntityWithoutNotifyingParent(Entity entity)
    {
        NC_ASSERT(m_entities.Contains(entity), "Bad Entity");
        auto* transform = Get<Transform>(entity);

        for(auto child : transform->Children())
            RemoveEntityWithoutNotifyingParent(child);

        m_entities.Remove(entity);
    }

    void Registry::Clear()
    {
        m_entities.Clear();

        for(auto& storage : m_registeredStorage)
        {
            storage->Clear();
        }
    }
}
