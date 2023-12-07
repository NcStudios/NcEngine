#pragma once

#include "ncengine/ecs/detail/HandleManager.h"
#include "ncengine/ecs/detail/PoolUtility.h"

#include "ncutility/NcError.h"

#include <algorithm>
#include <vector>

namespace nc::ecs
{
/** @brief */
class EntityPool : public std::ranges::view_interface<EntityPool>
{
    public:
        using value_type = Entity;
        using iterator = std::vector<Entity>::iterator;
        using const_iterator = std::vector<Entity>::const_iterator;
        using reverse_iterator = std::vector<Entity>::reverse_iterator;

        /** @brief */
        auto Add(Entity::layer_type layer, Entity::flags_type flags) -> Entity
        {
            auto handle = m_handleManager.GenerateNewHandle(layer, flags);
            m_entities.push_back(handle);
            if(handle.IsPersistent())
                m_persistent.push_back(handle);

            return handle;
        }

        /** @brief */
        void Remove(Entity entity)
        {
            m_toRemove.push_back(entity);
            detail::EraseUnstable(m_entities, entity);
            if (entity.IsPersistent())
                detail::EraseUnstable(m_persistent, entity);
        }

        /** @brief */
        bool Contains(Entity entity) const
        {
            return m_entities.cend() != std::ranges::find(m_entities, entity);
        }

        /** @brief */
        auto Size() const noexcept -> size_t { return m_entities.size(); }

        /** @brief Get a list of entities that have been removed but not yet recycled. */
        // TODO: should be range? span?
        auto GetDeadEntities() const -> const std::vector<Entity>& { return m_toRemove; }

        /** @brief Add removed entity indices back to the pool of possible indices. */
        void RecycleDeadEntities()
        {
            if(m_toRemove.empty()) return;
            m_handleManager.ReclaimHandles(m_toRemove);
            m_toRemove.clear();
        }

        /** @brief */
        void ClearNonPersistent()
        {
            m_toRemove.shrink_to_fit();
            m_persistent.shrink_to_fit();
            m_entities = m_persistent;
            m_handleManager.Reset(m_persistent);
        }

        /** @brief */
        void Clear()
        {
            m_entities.clear();
            m_entities.shrink_to_fit();
            m_toRemove.clear();
            m_toRemove.shrink_to_fit();
            m_persistent.clear();
            m_persistent.shrink_to_fit();
            m_handleManager.Reset({});
        }

        /** @brief Get an iterator to the first entity in the pool. (enables view_interface) */
        auto begin() noexcept { return std::ranges::begin(m_entities); }

        /** @brief Get a const_iterator to the first entity in the pool. (enables view_interface) */
        auto begin() const noexcept { return std::ranges::begin(m_entities); }

        /** @brief Get an iterator one past the last entity in the pool. (enables view_interface) */
        auto end() noexcept { return std::ranges::end(m_entities); }

        /** @brief Get a const_iterator one past the last entity in the pool. (enables view_interface) */
        auto end() const noexcept { return std::ranges::end(m_entities); }

        /** @brief 
         * 
         * empty()
         * operator bool
         * data
         * size
         * front
         * back
         * operator[]
        */

    private:
        std::vector<Entity> m_entities;
        std::vector<Entity> m_toRemove;
        std::vector<Entity> m_persistent;
        detail::HandleManager m_handleManager;
};
} // namespace nc::ecs::detail
