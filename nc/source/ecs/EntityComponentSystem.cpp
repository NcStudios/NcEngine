#include "EntityComponentSystem.h"

namespace nc::ecs
{
    #ifdef USE_VULKAN
    EntityComponentSystem::EntityComponentSystem(const config::MemorySettings& memSettings)
    #else
    EntityComponentSystem::EntityComponentSystem(graphics::Graphics* graphics, const config::MemorySettings& memSettings)
    #endif
        : m_registry{memSettings.maxTransforms},
          #ifdef USE_VULKAN
          m_particleEmitterSystem{&m_registry}
          #else
          m_particleEmitterSystem{&m_registry, graphics}
          #endif
    {
        internal::SetActiveRegistry(&m_registry);
    }

    void EntityComponentSystem::Clear()
    {
        m_registry.Clear();
        m_particleEmitterSystem.Clear();
    }
} // end namespace nc::ecs
