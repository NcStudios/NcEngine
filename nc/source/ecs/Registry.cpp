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
        for(auto entity : m_toAdd)
            Get<AutoComponentGroup>(entity)->SendOnDestroy();
        for(auto entity : m_toRemove)
            Get<AutoComponentGroup>(entity)->SendOnDestroy();
        for(auto entity : m_active)
            Get<AutoComponentGroup>(entity)->SendOnDestroy();

        m_active.clear();
        m_active.shrink_to_fit();
        m_toAdd.clear();
        m_toAdd.shrink_to_fit();
        m_toRemove.clear();
        m_toRemove.shrink_to_fit();

        for(auto& storage : m_registeredStorage)
            storage->Clear();
        
        m_handleManager.Reset();
    }

    void Registry::VerifyCallbacks()
    {
        for(auto& storage : m_registeredStorage)
            storage->VerifyCallbacks();
    }
}