#include "ecs/Registry.h"
#include "ecs/component/AudioSource.h"
#include "ecs/component/AutoComponentGroup.h"
#include "ecs/component/Collider.h"
#include "ecs/component/ConcaveCollider.h"
#include "ecs/component/NetworkDispatcher.h"
#include "ecs/component/ParticleEmitter.h"
#include "ecs/component/PhysicsBody.h"
#include "ecs/component/Tag.h"
#include "ecs/component/Transform.h"
#include "ecs/component/MeshRenderer.h"
#include "ecs/component/PointLight.h"

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
        RegisterComponentType<AutoComponentGroup>();
        RegisterComponentType<Collider>();
        RegisterComponentType<ConcaveCollider>();
        RegisterComponentType<NetworkDispatcher>();
        RegisterComponentType<ParticleEmitter>();
        RegisterComponentType<PhysicsBody>();
        RegisterComponentType<Tag>();
        RegisterComponentType<Transform>();
        RegisterComponentType<MeshRenderer>();
        RegisterComponentType<PointLight>();
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
            else
                Get<AutoComponentGroup>(entity)->SendOnDestroy();
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