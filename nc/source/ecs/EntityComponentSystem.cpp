#include "EntityComponentSystem.h"
#include "Ecs.h"
#include "component/Collider.h"
#include "component/NetworkDispatcher.h"
#include "component/PointLight.h"
#include "component/PointLightManager.h"
#include "component/Renderer.h"
#include "component/Transform.h"
#include "config/Config.h"

namespace nc::ecs
{
    #ifdef USE_VULKAN
    EntityComponentSystem::EntityComponentSystem()
    #else
    EntityComponentSystem::EntityComponentSystem(graphics::Graphics* graphics)
    #endif
        : m_handleManager{},
          m_activePool{config::GetMemorySettings().maxTransforms},
          m_toDestroy{},
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

    std::span<Entity*> EntityComponentSystem::GetActiveEntities() noexcept
    {
        return m_activePool.GetActiveRange();
    }

    EntityHandle EntityComponentSystem::CreateEntity(EntityInfo info)
    {
        auto entityHandle = m_handleManager.GenerateNewHandle();
        m_transformSystem->Add(entityHandle, info.position, info.rotation, info.scale, info.parent);
        m_activePool.Add(entityHandle, std::move(info.tag), info.layer, info.isStatic);
        return entityHandle;
    }

    bool EntityComponentSystem::DoesEntityExist(const EntityHandle handle) const noexcept
    {
        return m_activePool.Contains([handle](auto* e) { return e->Handle == handle; });
    }

    /** Friendly reminder - this invalidates m_active iterators */
    void EntityComponentSystem::DestroyEntity(EntityHandle handle)
    {
        m_toDestroy.push_back(m_activePool.Extract([handle](auto* e) { return e->Handle == handle; }));
    }

    Entity* EntityComponentSystem::GetEntity(EntityHandle handle)
    {
        return m_activePool.Get([handle](auto* e) { return e->Handle == handle; });
    }

    Entity* EntityComponentSystem::GetEntity(const std::string& tag)
    {
        return m_activePool.Get([&tag](auto* e) { return e->Tag == tag; });
    }

    void EntityComponentSystem::SendFrameUpdate(float dt)
    {
        for(auto* entity : m_activePool.GetActiveRange())
            entity->SendFrameUpdate(dt);
    }

    void EntityComponentSystem::SendFixedUpdate()
    {
        for(auto* entity : m_activePool.GetActiveRange())
            entity->SendFixedUpdate();
    }

    void EntityComponentSystem::SendOnDestroy()
    {
        for(auto& entity : m_toDestroy)
        {
            auto handle = entity.Handle;
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
        for(auto* entity : m_activePool.GetActiveRange())
            entity->SendOnDestroy();
        
        for(auto& entity : m_toDestroy)
            entity.SendOnDestroy();

        m_activePool.Clear();
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
