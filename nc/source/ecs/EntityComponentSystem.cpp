#include "EntityComponentSystem.h"
#include "Ecs.h"
#include "component/Collider.h"
#include "component/NetworkDispatcher.h"
#include "component/PointLight.h"
#include "component/PointLightManager.h"
#ifdef USE_VULKAN
#include "component/vulkan/MeshRenderer.h"
#include "graphics/Graphics2.h"
#endif
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
      m_colliderSystem{nullptr},
      m_lightSystem{ std::make_unique<ComponentSystem<PointLight>>(PointLightManager::MAX_POINT_LIGHTS) },
      m_rendererSystem{nullptr},
      m_transformSystem{nullptr},
      m_networkDispatcherSystem{nullptr}
{
    const auto& memorySettings = config::GetMemorySettings();
    const auto& physicsSettings = config::GetPhysicsSettings();

    m_colliderSystem = std::make_unique<ColliderSystem>
    (
        memorySettings.maxDynamicColliders,
        memorySettings.maxStaticColliders,
        physicsSettings.octreeDensityThreshold,
        physicsSettings.octreeMinimumExtent,
        physicsSettings.worldspaceExtent
    );

    m_rendererSystem = std::make_unique<ComponentSystem<Renderer>>(memorySettings.maxRenderers);
    m_transformSystem = std::make_unique<ComponentSystem<Transform>>(memorySettings.maxTransforms);
    m_networkDispatcherSystem = std::make_unique<ComponentSystem<NetworkDispatcher>>(memorySettings.maxNetworkDispatchers);

    internal::RegisterEcs(this);
}

#ifdef USE_VULKAN
EntityComponentSystem::EntityComponentSystem(nc::graphics::Graphics2* graphics)
    : m_handleManager{},
      m_active{InitialBucketSize, EntityHandle::Hash()},
      m_toDestroy{InitialBucketSize, EntityHandle::Hash()},
      m_colliderSystem{nullptr},
      m_lightSystem{ std::make_unique<ComponentSystem<PointLight>>(PointLightManager::MAX_POINT_LIGHTS) },
      m_meshRendererSystem{ std::make_unique<MeshRendererSystem>(config::GetMemorySettings().maxRenderers, graphics) },
      m_rendererSystem{nullptr},
      m_transformSystem{nullptr},
      m_networkDispatcherSystem{nullptr}
{
    const auto& memorySettings = config::GetMemorySettings();
    const auto& physicsSettings = config::GetPhysicsSettings();

    m_colliderSystem = std::make_unique<ColliderSystem>
    (
        memorySettings.maxDynamicColliders,
        memorySettings.maxStaticColliders,
        physicsSettings.octreeDensityThreshold,
        physicsSettings.octreeMinimumExtent,
        physicsSettings.worldspaceExtent
    );

    m_rendererSystem = std::make_unique<ComponentSystem<Renderer>>(memorySettings.maxRenderers);
    m_transformSystem = std::make_unique<ComponentSystem<Transform>>(memorySettings.maxTransforms);
    m_networkDispatcherSystem = std::make_unique<ComponentSystem<NetworkDispatcher>>(memorySettings.maxNetworkDispatchers);

    internal::RegisterEcs(this);
}
#endif

ColliderSystem* EntityComponentSystem::GetColliderSystem() const
{
    return m_colliderSystem.get();
}

ComponentSystem<NetworkDispatcher>* EntityComponentSystem::GetNetworkDispatcherSystem() const
{
    return m_networkDispatcherSystem.get();
}

ComponentSystem<PointLight>* EntityComponentSystem::GetPointLightSystem() const
{
    return m_lightSystem.get();
}

#ifdef USE_VULKAN
template<> ComponentSystem<vulkan::MeshRenderer>* EntityComponentSystem::GetSystem<vulkan::MeshRenderer>()
{
    return m_meshRendererSystem->GetSystem();
}

MeshRendererSystem* EntityComponentSystem::GetMeshRendererSystem()
{
    return m_meshRendererSystem.get();
}

#endif

ComponentSystem<Renderer>* EntityComponentSystem::GetRendererSystem() const
{
    return m_rendererSystem.get();
}

ComponentSystem<Transform>* EntityComponentSystem::GetTransformSystem() const
{
    return m_transformSystem.get();
}

Systems EntityComponentSystem::GetComponentSystems() const
{
    return Systems
    {
        .collider = m_colliderSystem->GetComponentSystem(),
        .networkDispatcher = m_networkDispatcherSystem.get(),
        .pointLight = m_lightSystem.get(),
        .renderer = m_rendererSystem.get(),
        .transform = m_transformSystem.get()
    };
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
        m_colliderSystem->Remove(handle, entity.IsStatic);
        m_transformSystem->Remove(handle);
        #ifdef USE_VULKAN
        m_meshRendererSystem->Remove(handle);
        #endif
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
    m_colliderSystem->Clear();
    m_transformSystem->Clear();
    #ifdef USE_VULKAN
    m_meshRendererSystem->Clear();
    #endif
    m_rendererSystem->Clear();
    m_lightSystem->Clear();
    m_networkDispatcherSystem->Clear();
}
} // end namespace nc::ecs
