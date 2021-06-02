#pragma once

#include "Ecs.h"
#include "ColliderSystem.h"
#include "ParticleEmitterSystem.h"
#include "config/Config.h"

namespace nc::graphics { class Graphics; }

namespace nc::ecs
{
    class EntityComponentSystem
    {
        public:
            #ifdef USE_VULKAN
            EntityComponentSystem(const config::MemorySettings& memSettings,
                                  const config::PhysicsSettings& physSettings);
            #else
            EntityComponentSystem(graphics::Graphics* graphics,
                                  const config::MemorySettings& memSettings,
                                  const config::PhysicsSettings& physSettings);
            #endif

            auto GetRegistry() noexcept { return &m_registry; }
            auto GetColliderSystem() noexcept { return &m_colliderSystem; }
            auto GetParticleEmitterSystem() noexcept { return &m_particleEmitterSystem; }
            void Clear();

        private:
            ColliderSystem m_colliderSystem;
            ParticleEmitterSystem m_particleEmitterSystem;
            registry_type m_registry;
    };
} // namespace nc::ecs