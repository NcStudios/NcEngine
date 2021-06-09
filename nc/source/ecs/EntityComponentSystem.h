#pragma once

#include "Registry.h"
#include "ColliderSystem.h"
#include "ParticleEmitterSystem.h"
#include "MeshRendererSystem.h"
#include "PointLightSystem.h"
#include "component/Collider.h"
#include "component/NetworkDispatcher.h"
#include "component/PointLight.h"
#include "component/PointLightManager.h"
#include "component/Renderer.h"
#include "component/Transform.h"
#include "component/vulkan/PointLight.h"
#include "component/vulkan/MeshRenderer.h"
#include "config/Config.h"


namespace nc::graphics { class Graphics; }

namespace nc::ecs
{
    #ifdef USE_VULKAN
    using registry_type = ecs::Registry<Collider, NetworkDispatcher, ParticleEmitter, PointLight, Renderer, Transform, nc::vulkan::PointLight, nc::vulkan::MeshRenderer>;
    #else
    using registry_type = ecs::Registry<Collider, NetworkDispatcher, ParticleEmitter, PointLight, Renderer, Transform>;
    #endif

    class EntityComponentSystem
    {
        public:
            #ifdef USE_VULKAN
            EntityComponentSystem(graphics::Graphics2* graphics,
                                  const config::MemorySettings& memSettings,
                                  const config::PhysicsSettings& physSettings);
            #else
            EntityComponentSystem(graphics::Graphics* graphics,
                                  const config::MemorySettings& memSettings,
                                  const config::PhysicsSettings& physSettings);
            #endif

            auto GetRegistry() noexcept { return &m_registry; }
            auto GetColliderSystem() noexcept { return &m_colliderSystem; }
            auto GetParticleEmitterSystem() noexcept { return &m_particleEmitterSystem; }
            
            #ifdef USE_VULKAN
            PointLightSystem* GetPointLightSystem();
            MeshRendererSystem* GetMeshRendererSystem();
            #endif

            void Clear();

        private:
            registry_type m_registry;

            ColliderSystem m_colliderSystem;
            ParticleEmitterSystem m_particleEmitterSystem;

            #ifdef USE_VULKAN
            PointLightSystem m_pointLightSystem;
            MeshRendererSystem m_meshRendererSystem;
            #endif
    };
} // namespace nc::ecs