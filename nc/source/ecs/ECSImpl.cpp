#include "EcsImpl.h"
#include "Ecs.h"
#include "component/PointLightManager.h"

namespace nc::ecs
{
EcsImpl::EcsImpl()
    : m_active{},
      m_toDestroy{},
      m_colliderSystem{ std::make_unique<ComponentSystem<Collider>>() },
      m_lightSystem{ std::make_unique<ComponentSystem<PointLight>>(PointLightManager::MAX_POINT_LIGHTS, true) },
      m_rendererSystem{ std::make_unique<ComponentSystem<Renderer>>() },
      m_transformSystem{ std::make_unique<ComponentSystem<Transform>>() },
      m_networkDispatcherSystem{ std::make_unique<ComponentSystem<NetworkDispatcher>>() }
{
}

template<> ComponentSystem<Collider>* EcsImpl::GetSystem<Collider>()
{
    return m_colliderSystem.get();
}

template<> ComponentSystem<PointLight>* EcsImpl::GetSystem<PointLight>()
{
    return m_lightSystem.get();
}

template<> ComponentSystem<Renderer>* EcsImpl::GetSystem<Renderer>()
{
    return m_rendererSystem.get();
}

template<> ComponentSystem<Transform>* EcsImpl::GetSystem<Transform>()
{
    return m_transformSystem.get();
}

template<> ComponentSystem<NetworkDispatcher>* EcsImpl::GetSystem<NetworkDispatcher>()
{
    return m_networkDispatcherSystem.get();
}

void EcsImpl::SendFrameUpdate(float dt)
{
    for(auto& pair : m_active)
    {
        pair.second.SendFrameUpdate(dt);
    }
}

void EcsImpl::SendFixedUpdate()
{
    for(auto& pair : m_active)
    {
        pair.second.SendFixedUpdate();
    }
}

void EcsImpl::SendOnDestroy()
{
    for(auto & pair : m_toDestroy)
    {
        Entity* entityPtr = GetEntityPtrFromAnyMap(pair.second.Handle);
        if (entityPtr == nullptr)
        {
            continue;
        }

        pair.second.SendOnDestroy();
        //const auto& handles = entityPtr->Handles;
        m_transformSystem->Remove(pair.first);
        m_rendererSystem->Remove(pair.first);
        m_lightSystem->Remove(pair.first);
    }
    m_toDestroy.clear();
}

bool EcsImpl::DoesEntityExist(const EntityHandle handle) const noexcept
{
    return m_active.count(handle) > 0;
}

std::unordered_map<EntityHandle, Entity> & EcsImpl::GetMapContainingEntity(const EntityHandle handle, bool checkAll) noexcept(false)
{
    if (m_active.count(handle) > 0)
        return m_active;

    if (checkAll && (m_toDestroy.count(handle) > 0) ) //only check toDestroy if checkAll flag is set
        return m_toDestroy;

    throw std::runtime_error("Entity not found.");
}

std::unordered_map<EntityHandle, Entity> & EcsImpl::GetActiveEntities() noexcept
{
    return m_active;
}

std::unordered_map<EntityHandle, Entity> & EcsImpl::GetToDestroyEntities() noexcept
{
    return m_toDestroy;
}

Entity * EcsImpl::GetEntityPtrFromAnyMap(const EntityHandle handle) noexcept(false)
{
    return &GetMapContainingEntity(handle, true).at(handle);
}

void EcsImpl::ClearState()
{
    for(const auto& pair : m_active)
    {
        Ecs::DestroyEntity(pair.first);
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