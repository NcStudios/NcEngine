#include "EntityComponentSystem.h"
#include "component/Collider.h"
#include "component/NetworkDispatcher.h"
#include "component/PointLight.h"
#include "component/PointLightManager.h"
#include "component/Renderer.h"
#include "component/Transform.h"
#include "config/Config.h"

namespace
{
    constexpr size_t InitialBucketSize = 10u;
}

namespace nc::ecs
{
EntityComponentSystem::EntityComponentSystem()
    : m_handleManager{},
      m_active{InitialBucketSize, EntityHandle::Hash()},
      m_toDestroy{InitialBucketSize, EntityHandle::Hash()},
      m_lightSystem{ std::make_unique<ComponentSystem<PointLight>>(PointLightManager::MAX_POINT_LIGHTS) },
      m_rendererSystem{ std::make_unique<ComponentSystem<Renderer>>(config::Get().memory.maxRenderers) },
      m_transformSystem{ std::make_unique<ComponentSystem<Transform>>(config::Get().memory.maxTransforms) },
      m_networkDispatcherSystem{ std::make_unique<ComponentSystem<NetworkDispatcher>>(config::Get().memory.maxNetworkDispatchers) }
{
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

EntityMap& EntityComponentSystem::GetActiveEntities() noexcept
{
    return m_active;
}

EntityHandle EntityComponentSystem::CreateEntity(EntityInfo info)
{
    auto entityHandle = m_handleManager.GenerateNewHandle();
    m_transformSystem->Add(entityHandle, info.position, info.rotation, info.scale);
    m_active.emplace(entityHandle, Entity{entityHandle, std::move(info.tag), info.layer, info.isStatic} );
    return entityHandle;
}

bool EntityComponentSystem::DoesEntityExist(const EntityHandle handle) const noexcept
{
    return m_active.count(handle) > 0;
}

/** Friendly reminder - this invalidates m_active iterators */
bool EntityComponentSystem::DestroyEntity(EntityHandle handle)
{
    if(!DoesEntityExist(handle))
        return false;

    m_toDestroy.insert(m_active.extract(handle));
    return true;
}

Entity* EntityComponentSystem::GetEntity(EntityHandle handle)
{
    if (!DoesEntityExist(handle))
        return nullptr;

    return &m_active.at(handle);
}

Entity* EntityComponentSystem::GetEntity(const std::string& tag)
{
    for(auto& [handle, entity] : m_active)
    {
        if(tag == entity.Tag)
            return &entity;
    }

    return nullptr;
}

void EntityComponentSystem::SendFrameUpdate(float dt)
{
    for(auto& [handle, entity] : m_active)
    {
        entity.SendFrameUpdate(dt);
    }
}

void EntityComponentSystem::SendFixedUpdate()
{
    for(auto& [handle, entity] : m_active)
    {
        entity.SendFixedUpdate();
    }
}

void EntityComponentSystem::SendOnDestroy()
{
    for(auto& [handle, entity] : m_toDestroy)
    {
        entity.SendOnDestroy();
        m_transformSystem->Remove(handle);
        m_rendererSystem->Remove(handle);
        m_lightSystem->Remove(handle);
        m_networkDispatcherSystem->Remove(handle);
    }

    m_toDestroy.clear();
}

void EntityComponentSystem::ClearState()
{
    // We cannot call DestroyEntity while iterating m_active, so copy the handles
    std::vector<EntityHandle> handles;
    handles.reserve(m_active.size());
    std::transform(m_active.cbegin(), m_active.cend(), std::back_inserter(handles), [](const auto& pair)
    {
        return pair.first;
    });

    for(const auto handle : handles)
        DestroyEntity(handle);

    SendOnDestroy();
    m_active.clear();
    m_toDestroy.clear();
    m_handleManager.Reset();
    m_transformSystem->Clear();
    m_rendererSystem->Clear();
    m_lightSystem->Clear();
    m_networkDispatcherSystem->Clear();
}
} // end namespace nc::ecs
