#pragma once

#include "Ecs.h"
#include "ParticleEmitterSystem.h"
#include "config/Config.h"

namespace nc::graphics { class Graphics; }

namespace nc::ecs
{
    class EntityComponentSystem
    {
        public:
            #ifdef USE_VULKAN
            EntityComponentSystem(const config::MemorySettings& memSettings);
            #else
            EntityComponentSystem(graphics::Graphics* graphics, const config::MemorySettings& memSettings);
            #endif

            auto GetRegistry() noexcept { return &m_registry; }
            auto GetParticleEmitterSystem() noexcept { return &m_particleEmitterSystem; }
            void Clear();

        private:
            registry_type m_registry;
            ParticleEmitterSystem m_particleEmitterSystem;
    };
} // namespace nc::ecs