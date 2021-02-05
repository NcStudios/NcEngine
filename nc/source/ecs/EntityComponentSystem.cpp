#include "EntityComponentSystem.h"
#include "math/Vector3.h"
#include "math/Quaternion.h"
#include "component/Collider.h"
#include "component/NetworkDispatcher.h"
#include "component/PointLight.h"
#include "component/PointLightManager.h"
#include "component/Renderer.h"
#include "component/Transform.h"

namespace
{
    constexpr size_t InitialBucketSize = 10u;
}

namespace nc::ecs
{
EntityComponentSystem::EntityComponentSystem()
    : m_active{InitialBucketSize, EntityHandle::Hash()},
      m_toDestroy{InitialBucketSize, EntityHandle::Hash()},
      m_colliderSystem{ std::make_unique<ComponentSystem<Collider>>() },
      m_lightSystem{ std::make_unique<ComponentSystem<PointLight>>(PointLightManager::MAX_POINT_LIGHTS, true) },
      m_rendererSystem{ std::make_unique<ComponentSystem<Renderer>>() },
      m_transformSystem{ std::make_unique<ComponentSystem<Transform>>() },
      m_networkDispatcherSystem{ std::make_unique<ComponentSystem<NetworkDispatcher>>() }
{
}

template<> ComponentSystem<Collider>* EntityComponentSystem::GetSystem<Collider>()
{
    return m_colliderSystem.get();
}

template<> ComponentSystem<PointLight>* EntityComponentSystem::GetSystem<PointLight>()
{
    return m_lightSystem.get();
}

template<> ComponentSystem<Renderer>* EntityComponentSystem::GetSystem<Renderer>()
{
    return m_rendererSystem.get();
}

template<> ComponentSystem<Transform>* EntityComponentSystem::GetSystem<Transform>()
{
    return m_transformSystem.get();
}

template<> ComponentSystem<NetworkDispatcher>* EntityComponentSystem::GetSystem<NetworkDispatcher>()
{
    return m_networkDispatcherSystem.get();
}

void EntityComponentSystem::SendFrameUpdate(float dt)
{
    for(auto& pair : m_active)
    {
        pair.second.SendFrameUpdate(dt);
    }
}

void EntityComponentSystem::SendFixedUpdate()
{
    for(auto& pair : m_active)
    {
        pair.second.SendFixedUpdate();
    }
}

void EntityComponentSystem::SendOnDestroy()
{
    for(auto & pair : m_toDestroy)
    {
        Entity* entityPtr = GetEntityPtrFromAnyMap(pair.second.Handle);
        if (entityPtr == nullptr)
        {
            continue;
        }

        pair.second.SendOnDestroy();
        m_transformSystem->Remove(pair.first);
        m_rendererSystem->Remove(pair.first);
        m_lightSystem->Remove(pair.first);
    }
    m_toDestroy.clear();
}

EntityHandle EntityComponentSystem::CreateEntity(Vector3 pos, Quaternion rot, Vector3 scale, std::string tag)
{
    auto entityHandle = m_handleManager.GenerateNewHandle();
    m_transformSystem->Add(entityHandle, pos, rot, scale);
    m_active.emplace(entityHandle, Entity{entityHandle, std::move(tag)} );
    return entityHandle;
}

bool EntityComponentSystem::DoesEntityExist(const EntityHandle handle) const noexcept
{
    return m_active.count(handle) > 0;
}

bool EntityComponentSystem::DestroyEntity(EntityHandle handle)
{
    if (DoesEntityExist(handle))
        return false;
    auto& containingMap = GetMapContainingEntity(handle);
    GetToDestroyEntities().emplace(handle, std::move(containingMap.at(handle)));
    containingMap.erase(handle);
    return true;
}

Entity* EntityComponentSystem::GetEntity(EntityHandle handle)
{
    if (!DoesEntityExist(handle))
        return nullptr;

    auto& containingMap = GetMapContainingEntity(handle);
    return &containingMap.at(handle); 
}

Entity* EntityComponentSystem::GetEntity(const std::string& tag)
{
    for(auto& pair : m_active)
    {
        if(tag == pair.second.Tag)
        {
            return &pair.second;
        }
    }
    return nullptr;
}

EntityMap& EntityComponentSystem::GetMapContainingEntity(const EntityHandle handle, bool checkAll)
{
    if (m_active.count(handle) > 0)
        return m_active;

    if (checkAll && (m_toDestroy.count(handle) > 0) ) //only check toDestroy if checkAll flag is set
        return m_toDestroy;

    throw std::runtime_error("Entity not found.");
}

EntityMap& EntityComponentSystem::GetActiveEntities() noexcept
{
    return m_active;
}

EntityMap& EntityComponentSystem::GetToDestroyEntities() noexcept
{
    return m_toDestroy;
}

Entity* EntityComponentSystem::GetEntityPtrFromAnyMap(const EntityHandle handle)
{
    return &GetMapContainingEntity(handle, true).at(handle);
}

void EntityComponentSystem::ClearState()
{
    for(const auto& pair : m_active)
    {
        DestroyEntity(pair.first);
    }

    SendOnDestroy();
    m_active.clear();
    m_toDestroy.clear();
    m_handleManager.Reset();
    m_colliderSystem->Clear();
    m_transformSystem->Clear();
    m_rendererSystem->Clear();
    m_lightSystem->Clear();
    m_networkDispatcherSystem->Clear();
}
} // end namespace nc::ecs
