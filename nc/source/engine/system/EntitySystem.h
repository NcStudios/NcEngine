#pragma once
#include "NcCommon.h"
#include "HandleManager.h"

#include <unordered_map>

namespace nc::engine::system
{
    class EntitySystem
    {
        using EntityMap = std::unordered_map<EntityHandle, Entity>;

        public:
            bool DoesEntityExist(const EntityHandle handle) noexcept;
            Entity * GetEntityPtrFromAnyMap(const EntityHandle handle) noexcept(false);
            EntityMap & GetMapContainingEntity(const EntityHandle handle, bool checkAll = false) noexcept(false);
            EntityMap & GetActiveEntities() noexcept;
            EntityMap & GetToDestroyEntities() noexcept;

            HandleManager<EntityHandle> handleManager;
            
        private:
            EntityMap m_active;
            EntityMap m_toDestroy;
    };

    bool EntitySystem::DoesEntityExist(const EntityHandle handle) noexcept
    {
        return m_active.count(handle) > 0;
    }

    std::unordered_map<EntityHandle, Entity> & EntitySystem::GetMapContainingEntity(const EntityHandle handle, bool checkAll) noexcept(false)
    {
        if (m_active.count(handle) > 0)
            return m_active;

        if (checkAll && (m_toDestroy.count(handle) > 0) ) //only check toDestroy if checkAll flag is set
            return m_toDestroy;

        throw std::runtime_error("Engine::GetmapContainingEntity() - Entity not found.");
    }

    std::unordered_map<EntityHandle, Entity> & EntitySystem::GetActiveEntities() noexcept
    {
        return m_active;
    }

    std::unordered_map<EntityHandle, Entity> & EntitySystem::GetToDestroyEntities() noexcept
    {
        return m_toDestroy;
    }

    Entity * EntitySystem::GetEntityPtrFromAnyMap(const EntityHandle handle) noexcept(false)
    {
        return &GetMapContainingEntity(handle, true).at(handle);
    }
}