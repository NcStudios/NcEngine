#include "EntityComponentSystem.h"

namespace nc::ecs
{
    EntityComponentSystem::EntityComponentSystem(graphics::Graphics* graphics,
                                                 const config::MemorySettings& memSettings)
        : m_registry{memSettings.maxTransforms},
          m_particleEmitterSystem{&m_registry, graphics},
          m_pointLightSystem{&m_registry, graphics}
    {
    }

    void EntityComponentSystem::Clear()
    {
        m_registry.Clear();
        m_particleEmitterSystem.Clear();
        m_pointLightSystem.Clear();
    }
} // end namespace nc::ecs
