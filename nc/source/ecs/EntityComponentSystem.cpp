#include "EntityComponentSystem.h"

namespace nc::ecs
{
    #ifdef USE_VULKAN
    EntityComponentSystem::EntityComponentSystem(const config::MemorySettings& memSettings,
                                                 const config::PhysicsSettings& physSettings)
    #else
    EntityComponentSystem::EntityComponentSystem(graphics::Graphics* graphics,
                                                 const config::MemorySettings& memSettings,
                                                 const config::PhysicsSettings& physSettings)
    #endif
        // : m_entitySystem{memSettings.maxTransforms},
        //   m_colliderSystem{memSettings.maxDynamicColliders,
        //                    memSettings.maxStaticColliders,
        //                    physSettings.octreeDensityThreshold,
        //                    physSettings.octreeMinimumExtent,
        //                    physSettings.worldspaceExtent},
        //   m_lightSystem{PointLightManager::MAX_POINT_LIGHTS},
        //   #ifdef USE_VULKAN
        //   m_particleEmitterSystem{memSettings.maxParticleEmitters},
        //   #else
        //   m_particleEmitterSystem{memSettings.maxParticleEmitters, graphics},
        //   #endif
        //   m_rendererSystem{memSettings.maxRenderers},
        //   m_transformSystem{memSettings.maxTransforms},
        //   m_networkDispatcherSystem{memSettings.maxNetworkDispatchers}
        : m_entitySystem{memSettings.maxTransforms},
          m_colliderSystem{memSettings.maxTransforms,
                           memSettings.maxStaticColliders,
                           physSettings.octreeDensityThreshold,
                           physSettings.octreeMinimumExtent,
                           physSettings.worldspaceExtent},
          m_lightSystem{memSettings.maxTransforms},
          #ifdef USE_VULKAN
          m_particleEmitterSystem{memSettings.maxTransforms},
          #else
          m_particleEmitterSystem{memSettings.maxTransforms, graphics},
          #endif
          m_rendererSystem{memSettings.maxTransforms},
          m_transformSystem{memSettings.maxTransforms},
          m_networkDispatcherSystem{memSettings.maxTransforms}
    {
        internal::RegisterEcs(this);
    }

    Systems EntityComponentSystem::GetComponentSystems() noexcept
    {
        return Systems
        {
            .collider = m_colliderSystem.GetComponentSystem(),
            .networkDispatcher = &m_networkDispatcherSystem,
            .particleEmitter = m_particleEmitterSystem.GetComponentSystem(),
            .pointLight = &m_lightSystem,
            .renderer = &m_rendererSystem,
            .transform = &m_transformSystem
        };
    }

    void EntityComponentSystem::FrameEnd()
    {
        m_entitySystem.CommitRemovals([this](const auto& entity)
        {
            auto handle = entity.Handle;
            m_colliderSystem.Remove(handle);
            m_transformSystem.Remove(handle);
            m_rendererSystem.Remove(handle);
            m_lightSystem.Remove(handle);
            m_networkDispatcherSystem.Remove(handle);
            m_particleEmitterSystem.Remove(handle);
        });
    }

    void EntityComponentSystem::Clear()
    {
        m_entitySystem.Clear();
        m_colliderSystem.Clear();
        m_transformSystem.Clear();
        m_rendererSystem.Clear();
        m_lightSystem.Clear();
        m_networkDispatcherSystem.Clear();
        m_particleEmitterSystem.Clear();
    }
} // end namespace nc::ecs
