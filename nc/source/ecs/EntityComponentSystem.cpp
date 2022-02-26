#include "EntityComponentSystem.h"

namespace nc::ecs
{
    EntityComponentSystem::EntityComponentSystem(const config::MemorySettings& memSettings)
        : m_registry{memSettings.maxTransforms},
          m_particleEmitterSystem{&m_registry},
          m_pointLightSystem{&m_registry}
    {
    }

    void EntityComponentSystem::Clear()
    {
        m_registry.Clear();
        m_particleEmitterSystem.Clear();
        m_pointLightSystem.Clear();
    }
} // end namespace nc::ecs
