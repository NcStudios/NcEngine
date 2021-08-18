#pragma once

#include "Ecs.h"
#include "ParticleEmitterSystem.h"
#include "MeshRendererSystem.h"
#include "PointLightSystem.h"
#include "config/Config.h"

namespace nc::graphics { class Graphics; class Graphics2; }

namespace nc::ecs
{
    class EntityComponentSystem
    {
        public:
            #ifdef USE_VULKAN
            EntityComponentSystem(graphics::Graphics2* graphics,
                                  const config::MemorySettings& memSettings);
            #else
            EntityComponentSystem(graphics::Graphics* graphics, const config::MemorySettings& memSettings);
            #endif

            auto GetRegistry() noexcept { return &m_registry; }
            auto GetParticleEmitterSystem() noexcept { return &m_particleEmitterSystem; }
            
            #ifdef USE_VULKAN
            auto GetMeshRendererSystem() noexcept { return &m_meshRendererSystem; }
            auto GetPointLightSystem() noexcept { return &m_pointLightSystem; }
            #endif

            void Clear();

        private:
            registry_type m_registry;
            ParticleEmitterSystem m_particleEmitterSystem;

            #ifdef USE_VULKAN
            MeshRendererSystem m_meshRendererSystem;
            PointLightSystem m_pointLightSystem;
            #endif
    };
} // namespace nc::ecs