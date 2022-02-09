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
          m_active{},
          m_toAdd{},
          m_toRemove{},
          m_handleManager{},
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
        IF_THROW(!Contains<Entity>(entity), "Bad Entity");
        auto* transform = Get<Transform>(entity);
        for(auto child : transform->Children())
            RemoveEntityWithoutNotifyingParent(child);
        
        auto pos = std::ranges::find(m_active, entity);
        *pos = m_active.back();
        m_active.pop_back();
        m_toRemove.push_back(entity);
    }

    void Registry::Clear()
    {
        if(!m_toAdd.empty() || !m_toRemove.empty())
        {
            CommitStagedChanges();
            m_toAdd.clear();
            m_toRemove.clear();
        }

        m_toAdd.shrink_to_fit();
        m_toRemove.shrink_to_fit();

        std::vector<Entity> persistentEntities;

        for(auto entity : m_active)
        {
            if(entity.IsPersistent())
                persistentEntities.push_back(entity);
        }

        m_active = std::move(persistentEntities);
        m_handleManager.Reset(m_active);

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