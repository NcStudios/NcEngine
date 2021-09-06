#pragma once

#include "Ecs.h"
#include "ParticleEmitterSystem.h"
#include "MeshRendererSystem.h"
#include "PointLightSystem.h"
#include "config/Config.h"

namespace nc::graphics { class Graphics2; }

namespace nc::ecs
{
    class EntityComponentSystem
    {
        public:
            EntityComponentSystem(graphics::Graphics2* graphics,
                                  const config::MemorySettings& memSettings);
            auto GetRegistry() noexcept { return &m_registry; }
            auto GetParticleEmitterSystem() noexcept { return &m_particleEmitterSystem; }
            auto GetMeshRendererSystem() noexcept { return &m_meshRendererSystem; }
            auto GetPointLightSystem() noexcept { return &m_pointLightSystem; }

            void Clear();

        private:
            registry_type m_registry;
            ParticleEmitterSystem m_particleEmitterSystem;
            MeshRendererSystem m_meshRendererSystem;
            PointLightSystem m_pointLightSystem;
    };
} // namespace nc::ecs