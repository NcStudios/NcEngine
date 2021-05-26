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
        : m_colliderSystem{memSettings.maxTransforms,
                           memSettings.maxStaticColliders,
                           physSettings.octreeDensityThreshold,
                           physSettings.octreeMinimumExtent,
                           physSettings.worldspaceExtent},
          #ifdef USE_VULKAN
          m_particleEmitterSystem{},
          #else
          m_particleEmitterSystem{graphics},
          #endif
          m_registry{memSettings.maxTransforms}
    {
        internal::RegisterEcs(this);

        auto* colliderSystem = &m_colliderSystem;
        m_registry.RegisterOnAddCallback<Collider>
        (
            [colliderSystem](const Collider& c) { colliderSystem->Add(c); }
        );

        m_registry.RegisterOnRemoveCallback<Collider>
        (
            [colliderSystem](EntityHandle h) { colliderSystem->Remove(h); }
        );

        auto* particleEmitterSystem = &m_particleEmitterSystem;
        m_registry.RegisterOnAddCallback<ParticleEmitter>
        (
            [particleEmitterSystem](const ParticleEmitter& pe) { particleEmitterSystem->Add(pe); }
        );

        m_registry.RegisterOnRemoveCallback<ParticleEmitter>
        (
            [particleEmitterSystem](EntityHandle h) { particleEmitterSystem->Remove(h); }
        );
    }

    void EntityComponentSystem::Clear()
    {
        m_registry.Clear();
        m_colliderSystem.Clear();
        m_particleEmitterSystem.Clear();
    }
} // end namespace nc::ecs
