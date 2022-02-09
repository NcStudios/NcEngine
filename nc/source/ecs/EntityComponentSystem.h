#pragma once

#include "ecs/Registry.h"
#include "ParticleEmitterSystem.h"
#include "PointLightSystem.h"
#include "config/Config.h"

namespace nc::graphics { class Graphics; }

namespace nc::ecs
{
    class EntityComponentSystem
    {
        public:
            EntityComponentSystem(graphics::Graphics* graphics,
                                  const config::MemorySettings& memSettings);

            auto GetRegistry() noexcept { return &m_registry; }
            auto GetParticleEmitterSystem() noexcept { return &m_particleEmitterSystem; }
            auto GetPointLightSystem() noexcept { return &m_pointLightSystem; }

            void Clear();

        private:
            Registry m_registry;
            ParticleEmitterSystem m_particleEmitterSystem;
            PointLightSystem m_pointLightSystem;
    };
} // namespace nc::ecs