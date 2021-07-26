#include "EntityComponentSystem.h"
#include "graphics/Model.h" // @todo: Hack - remove

namespace nc::ecs
{
    #ifdef USE_VULKAN
    EntityComponentSystem::EntityComponentSystem(graphics::Graphics2* graphics,
                                                 const config::MemorySettings& memSettings,
                                                 const config::PhysicsSettings& physSettings)
        : m_registry{memSettings.maxTransforms},
          m_colliderSystem{&m_registry,
                           memSettings.maxTransforms,
                           memSettings.maxStaticColliders,
                           physSettings.octreeDensityThreshold,
                           physSettings.octreeMinimumExtent,
                           physSettings.worldspaceExtent},
          m_particleEmitterSystem{&m_registry, graphics},
          m_debugWidgetSystem{&m_registry, graphics},
          m_meshRendererSystem{&m_registry, graphics},
          m_pointLightSystem{&m_registry, graphics}
    #else
    EntityComponentSystem::EntityComponentSystem(graphics::Graphics* graphics,
                                                 const config::MemorySettings& memSettings,
                                                 const config::PhysicsSettings& physSettings)
        : m_registry{memSettings.maxTransforms},
          m_colliderSystem{&m_registry,
                           memSettings.maxTransforms,
                           memSettings.maxStaticColliders,
                           physSettings.octreeDensityThreshold,
                           physSettings.octreeMinimumExtent,
                           physSettings.worldspaceExtent},
          m_particleEmitterSystem{&m_registry, graphics}
    #endif
    {
        internal::SetActiveRegistry(&m_registry);
        m_registry.VerifyCallbacks();
    }

    void EntityComponentSystem::Clear()
    {
        m_registry.Clear();
        m_colliderSystem.Clear();
        m_particleEmitterSystem.Clear();
        #ifdef USE_VULKAN
        m_pointLightSystem.Clear();
        m_meshRendererSystem.Clear();
        m_debugWidgetSystem.Clear();
        #endif
    }
} // end namespace nc::ecs
