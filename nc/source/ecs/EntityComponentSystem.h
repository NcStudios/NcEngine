#pragma once

#include "EntitySystem.h"
#include "ColliderSystem.h"
#include "ParticleEmitterSystem.h"
#include "alloc/PoolAdapter.h"
#include "component/Collider.h"
#include "component/NetworkDispatcher.h"
#include "component/PointLight.h"
#include "component/PointLightManager.h"
#include "component/Renderer.h"
#include "component/Transform.h"
#include "config/Config.h"

namespace nc::graphics { class Graphics; }

namespace nc::ecs
{
    struct Systems
    {
        ecs::ComponentSystem<Collider>* collider;
        ecs::ComponentSystem<NetworkDispatcher>* networkDispatcher;
        ecs::ComponentSystem<ParticleEmitter>* particleEmitter;
        ecs::ComponentSystem<PointLight>* pointLight;
        ecs::ComponentSystem<Renderer>* renderer;
        ecs::ComponentSystem<Transform>* transform;
    };

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

            auto GetColliderSystem() noexcept { return &m_colliderSystem; }
            auto GetNetworkDispatcherSystem() noexcept { return &m_networkDispatcherSystem; }
            auto GetParticleEmitterSystem() noexcept { return &m_particleEmitterSystem; }
            auto GetPointLightSystem() noexcept { return &m_lightSystem; }
            auto GetRendererSystem()  noexcept { return &m_rendererSystem; }
            auto GetTransformSystem() noexcept { return &m_transformSystem; }
            auto GetEntitySystem() noexcept { return &m_entitySystem; }
            auto GetActiveEntities() noexcept { return m_entitySystem.GetActiveEntities(); }
            auto GetComponentSystems() noexcept -> Systems;
            void FrameEnd();
            void Clear();

        private:
            EntitySystem m_entitySystem;
            ColliderSystem m_colliderSystem;
            ComponentSystem<PointLight> m_lightSystem;
            ParticleEmitterSystem m_particleEmitterSystem;
            ComponentSystem<Renderer> m_rendererSystem;
            ComponentSystem<Transform> m_transformSystem;
            ComponentSystem<NetworkDispatcher> m_networkDispatcherSystem;
    };
} // namespace nc::ecs