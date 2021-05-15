#include "EntityComponentSystem.h"
#include "Ecs.h"
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
#ifdef USE_VULKAN
EntityComponentSystem::EntityComponentSystem()
#else
EntityComponentSystem::EntityComponentSystem(graphics::Graphics* graphics)
#endif
    : m_handleManager{},
      m_active{InitialBucketSize, EntityHandle::Hash()},
      m_toDestroy{InitialBucketSize, EntityHandle::Hash()},
      m_colliderSystem{nullptr},
      m_lightSystem{ std::make_unique<ComponentSystem<PointLight>>(PointLightManager::MAX_POINT_LIGHTS) },
      m_particleEmitterSystem{nullptr},
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

    #ifdef USE_VULKAN
    m_particleEmitterSystem = std::make_unique<ParticleEmitterSystem>(memorySettings.maxParticleEmitters);
    #else
    m_particleEmitterSystem = std::make_unique<ParticleEmitterSystem>(memorySettings.maxParticleEmitters, graphics);
    #endif

    m_rendererSystem = std::make_unique<ComponentSystem<Renderer>>(memorySettings.maxRenderers);
    m_transformSystem = std::make_unique<ComponentSystem<Transform>>(memorySettings.maxTransforms);
    m_networkDispatcherSystem = std::make_unique<ComponentSystem<NetworkDispatcher>>(memorySettings.maxNetworkDispatchers);

    internal::RegisterEcs(this);
}

ColliderSystem* EntityComponentSystem::GetColliderSystem() const
{
    return m_colliderSystem.get();
}

ComponentSystem<NetworkDispatcher>* EntityComponentSystem::GetNetworkDispatcherSystem() const
{
    return m_networkDispatcherSystem.get();
}

ParticleEmitterSystem* EntityComponentSystem::GetParticleEmitterSystem()
{
    return m_particleEmitterSystem.get();
}

ComponentSystem<PointLight>* EntityComponentSystem::GetPointLightSystem() const
{
    return m_lightSystem.get();
}

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
        .particleEmitter = m_particleEmitterSystem->GetComponentSystem(),
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
    m_transformSystem->Add(entityHandle, info.position, info.rotation, info.scale, info.parent);
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
        m_rendererSystem->Remove(handle);
        m_lightSystem->Remove(handle);
        m_networkDispatcherSystem->Remove(handle);
        m_particleEmitterSystem->Remove(handle);
    }

    m_toDestroy.clear();
}

void EntityComponentSystem::ClearState()
{
    // Don't do the full SendOnDestroy process as systems will be cleared anyway.
    for(auto& [handle, entity] : m_active)
        entity.SendOnDestroy();
    
    for(auto& [handle, entity] : m_toDestroy)
        entity.SendOnDestroy();

    m_active.clear();
    m_toDestroy.clear();
    m_handleManager.Reset();
    m_colliderSystem->Clear();
    m_transformSystem->Clear();
    m_rendererSystem->Clear();
    m_lightSystem->Clear();
    m_networkDispatcherSystem->Clear();
    m_particleEmitterSystem->Clear();
}
} // end namespace nc::ecs
