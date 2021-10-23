#include "component/Registry.h"
#include "component/AudioSource.h"
#include "component/AutoComponentGroup.h"
#include "component/Collider.h"
#include "component/ConcaveCollider.h"
#include "component/NetworkDispatcher.h"
#include "component/ParticleEmitter.h"
#include "component/PhysicsBody.h"
#include "component/Registry.h"
#include "component/Tag.h"
#include "component/Transform.h"
#include "component/MeshRenderer.h"
#include "component/PointLight.h"

namespace nc
{
    Registry::Registry(size_t maxEntities)
        : m_registeredStorage{},
          m_active{},
          m_toAdd{},
          m_toRemove{},
          m_handleManager{},
          m_maxEntities{maxEntities}
    {
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