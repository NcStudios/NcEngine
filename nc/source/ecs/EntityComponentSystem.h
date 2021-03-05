#pragma once

#include "entity/Entity.h"
#include "entity/EntityHandle.h"
#include "entity/EntityInfo.h"
#include "ecs/EntityMap.h"
#include "ecs/ComponentSystem.h"
#include "HandleManager.h"

#include <memory>

namespace nc
{
    class Collider;
    class NetworkDispatcher;
    class PointLight;
    class Renderer;
    class Vector3;
    class Quaternion;

namespace ecs
{
    class EntityComponentSystem
    {
        public:
            EntityComponentSystem();

            template<std::derived_from<ComponentBase> T>
            ComponentSystem<T>* GetSystem();

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
            std::unique_ptr<ComponentSystem<Renderer>> m_rendererSystem;
            std::unique_ptr<ComponentSystem<Transform>> m_transformSystem;
            std::unique_ptr<ComponentSystem<NetworkDispatcher>> m_networkDispatcherSystem;
    };
}} // end namespace nc::ecs