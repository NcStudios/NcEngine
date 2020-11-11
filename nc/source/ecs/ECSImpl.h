#pragma once

#include "NcCommonTypes.h"
#include "ecs/Entity.h"
#include "ecs/ComponentSystem.h"
#include "HandleManager.h"

#include <memory>
#include <unordered_map>

namespace nc
{
    class ECS;
    class PointLight;
    class Renderer;
namespace ecs
{
    class ECSImpl
    {
        template<class T>
        using is_component_t = typename std::enable_if_t<std::is_base_of_v<Component, T>>;
        using EntityMap = std::unordered_map<EntityHandle, Entity>;
        friend class ::nc::ECS;

        public:
            ECSImpl();

            template<class T, class = is_component_t<T>>
            ComponentSystem<T>* GetSystem();

            void SendFrameUpdate(float dt);
            void SendFixedUpdate();
            void SendOnDestroy();

            bool DoesEntityExist(const EntityHandle handle) noexcept;
            Entity * GetEntityPtrFromAnyMap(const EntityHandle handle) noexcept(false);
            EntityMap & GetMapContainingEntity(const EntityHandle handle, bool checkAll = false) noexcept(false);
            EntityMap & GetActiveEntities() noexcept;
            EntityMap & GetToDestroyEntities() noexcept;

            void ClearState();

        private:
            HandleManager<EntityHandle> m_handleManager;
            EntityMap m_active;
            EntityMap m_toDestroy;
            std::unique_ptr<ComponentSystem<PointLight>> m_lightSystem;
            std::unique_ptr<ComponentSystem<Renderer>> m_rendererSystem;
            std::unique_ptr<ComponentSystem<Transform>> m_transformSystem;
    };
}} // end namespace nc::ecs