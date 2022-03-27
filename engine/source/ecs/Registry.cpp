#include "ecs/Registry.h"
#include "ecs/component/All.h"

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
        RegisterComponentType<AudioSource>();
        RegisterComponentType<FreeComponentGroup>();
        RegisterComponentType<Collider>();
        RegisterComponentType<CollisionLogic>();
        RegisterComponentType<ConcaveCollider>();
        RegisterComponentType<FrameLogic>();
        RegisterComponentType<FixedLogic>();
        RegisterComponentType<NetworkDispatcher>();
        RegisterComponentType<ParticleEmitter>();
        RegisterComponentType<PhysicsBody>();
        RegisterComponentType<Tag>();
        RegisterComponentType<Transform>();
        RegisterComponentType<MeshRenderer>();
        RegisterComponentType<PointLight>();
    }

    void Registry::RemoveEntityWithoutNotifyingParent(Entity entity)
    {
        NC_ASSERT(m_entities.contains(entity), "Bad Entity");
        auto* transform = Get<Transform>(entity);

        for(auto child : transform->Children())
            RemoveEntityWithoutNotifyingParent(child);

        m_entities.remove(entity);
    }

    void Registry::Clear()
    {
        m_entities.clear();

        for(auto& storage : m_registeredStorage)
        {
            storage->Clear();
        }
    }

    void Registry::VerifyCallbacks()
    {
        for(auto& storage : m_registeredStorage)
            storage->VerifyCallbacks();
    }
}