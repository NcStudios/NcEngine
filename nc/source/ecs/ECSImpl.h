#pragma once

#include "NcCommonTypes.h"
#include "ecs/Entity.h"
#include "HandleManager.h"

#include <unordered_map>

namespace nc { class ECS; }

namespace nc::ecs
{
    class LightSystem;
    class RenderingSystem;
    class TransformSystem;

    class ECSImpl
    {
        friend class ::nc::ECS;

        using EntityMap = std::unordered_map<EntityHandle, Entity>;

        public:
            ECSImpl(LightSystem* light, RenderingSystem* rend, TransformSystem* trans);

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
            LightSystem* m_lightSystem;
            RenderingSystem* m_renderingSystem;
            TransformSystem* m_transformSystem;

    };
}