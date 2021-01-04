#pragma once

#include "NcCommonTypes.h"
#include "ecs/Entity.h"
#include "ecs/ComponentSystem.h"
#include "HandleManager.h"

#include <memory>
#include <unordered_map>

namespace nc
{
    class Ecs;
    class Collider;
    class NetworkDispatcher;
    class PointLight;
    class Renderer;

namespace ecs
{
    class EcsImpl
    {
        using EntityMap = std::unordered_map<EntityHandle, Entity>;
        friend class ::nc::Ecs;

        public:
            EcsImpl();

            template<std::derived_from<Component> T>
            ComponentSystem<T>* GetSystem();

            void SendFrameUpdate(float dt);
            void SendFixedUpdate();
            void SendOnDestroy();

            bool DoesEntityExist(const EntityHandle handle) const noexcept;
            Entity * GetEntityPtrFromAnyMap(const EntityHandle handle) noexcept(false);
            EntityMap & GetMapContainingEntity(const EntityHandle handle, bool checkAll = false) noexcept(false);
            EntityMap & GetActiveEntities() noexcept;
            EntityMap & GetToDestroyEntities() noexcept;

            void ClearState();

        private:
            HandleManager<EntityHandle> m_handleManager;
            EntityMap m_active;
            EntityMap m_toDestroy;
            std::unique_ptr<ComponentSystem<Collider>> m_colliderSystem;
            std::unique_ptr<ComponentSystem<PointLight>> m_lightSystem;
            std::unique_ptr<ComponentSystem<Renderer>> m_rendererSystem;
            std::unique_ptr<ComponentSystem<Transform>> m_transformSystem;
            std::unique_ptr<ComponentSystem<NetworkDispatcher>> m_networkDispatcherSystem;
    };
}} // end namespace nc::ecs