#pragma once

#include "entity/Entity.h"
#include "entity/EntityHandle.h"
#include "entity/EntityInfo.h"
#include "ecs/EntityMap.h"
#include "ComponentSystem.h"
#include "HandleManager.h"
#ifdef USE_VULKAN
#include "RendererSystem.h"
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
}

namespace nc::ecs
{
    struct Systems
    {
        ecs::ComponentSystem<NetworkDispatcher>* networkDispatcher;
        ecs::ComponentSystem<PointLight>* pointLight;
        ecs::ComponentSystem<Renderer>* renderer;
        ecs::ComponentSystem<Transform>* transform;
        ecs::ComponentSystem<Collider>* collider;
    };

    class EntityComponentSystem
    {
        public:
            EntityComponentSystem();

            template<std::derived_from<ComponentBase> T>
            ComponentSystem<T>* GetSystem();

            #ifdef USE_VULKAN
            RendererSystem* GetRendererSystem();
            #endif

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
            std::unique_ptr<ComponentSystem<PointLight>> m_lightSystem;
            #ifdef USE_VULKAN
            std::unique_ptr<RendererSystem> m_rendererSystem2;
            #endif
            std::unique_ptr<ComponentSystem<Renderer>> m_rendererSystem;
            std::unique_ptr<ComponentSystem<Transform>> m_transformSystem;
            std::unique_ptr<ComponentSystem<NetworkDispatcher>> m_networkDispatcherSystem;
    };
} // namespace nc::ecs