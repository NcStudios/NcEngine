#pragma once

#include "entity/Entity.h"
#include "entity/EntityHandle.h"
#include "entity/EntityInfo.h"
#include "EntityMap.h"
#include "ComponentSystem.h"
#include "ColliderSystem.h"
#include "HandleManager.h"
#ifdef USE_VULKAN
#include "MeshRendererSystem.h"
#endif
#include <memory>

namespace nc
{
    class Collider;
    class NetworkDispatcher;
    class PointLight;
    class Renderer;
    class Vector3;
    class Quaternion;
    namespace physics { class ColliderSystem; }

    #ifdef USE_VULKAN
    namespace graphics { class Graphics2; }
    #endif
}

namespace nc::ecs
{
    struct Systems
    {
        ecs::ComponentSystem<Collider>* collider;
        ecs::ComponentSystem<NetworkDispatcher>* networkDispatcher;
        ecs::ComponentSystem<PointLight>* pointLight;
        ecs::ComponentSystem<Renderer>* renderer;
        ecs::ComponentSystem<Transform>* transform;
#ifdef USE_VULKAN
        ecs::ComponentSystem<MeshRenderer>* meshRenderer;
#endif
    };

    class EntityComponentSystem
    {
        public:
            #ifdef USE_VULKAN
            EntityComponentSystem(nc::graphics::Graphics2* graphics);
            MeshRendererSystem* GetMeshRendererSystem();
            #endif

            EntityComponentSystem();

            template<std::derived_from<ComponentBase> T>
            ComponentSystem<T>* GetSystem();
            ColliderSystem* GetColliderSystem() const;
            ComponentSystem<NetworkDispatcher>* GetNetworkDispatcherSystem() const;
            ComponentSystem<PointLight>* GetPointLightSystem() const;
            ComponentSystem<Renderer>* GetRendererSystem() const;
            ComponentSystem<Transform>* GetTransformSystem() const;
            Systems GetComponentSystems() const;
            EntityMap& GetActiveEntities() noexcept;

            EntityHandle CreateEntity(EntityInfo info);
            bool DestroyEntity(EntityHandle handle);
            bool DoesEntityExist(const EntityHandle handle) const noexcept;
            Entity* GetEntity(EntityHandle handle);
            Entity* GetEntity(const std::string& tag);

            void SendFrameUpdate(float dt);
            void SendFixedUpdate();
            void SendOnDestroy();
            void ClearState();

        private:
            HandleManager m_handleManager;
            EntityMap m_active;
            EntityMap m_toDestroy;
            std::unique_ptr<ColliderSystem> m_colliderSystem;
            std::unique_ptr<ComponentSystem<PointLight>> m_lightSystem;
            #ifdef USE_VULKAN
            std::unique_ptr<MeshRendererSystem> m_meshRendererSystem;
            #endif
            std::unique_ptr<ComponentSystem<Renderer>> m_rendererSystem;
            std::unique_ptr<ComponentSystem<Transform>> m_transformSystem;
            std::unique_ptr<ComponentSystem<NetworkDispatcher>> m_networkDispatcherSystem;
    };
} // namespace nc::ecs