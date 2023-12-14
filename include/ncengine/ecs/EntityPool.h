/**
 * @file EntityPool.h
 * @copyright Jaremie Romer and McCallister Romer 2023
 */
#pragma once

#include "ncengine/ecs/detail/HandleManager.h"
#include "ncengine/ecs/detail/PoolUtility.h"
#include "ncengine/type/StableAddress.h"

#include "ncutility/NcError.h"

#include <algorithm>
#include <vector>

namespace nc::ecs
{
/** @brief Storage class for tracking active entities. */
class EntityPool : public StableAddress
{
    public:
        using value_type = Entity;
        using iterator = std::vector<Entity>::iterator;
        using const_iterator = std::vector<Entity>::const_iterator;
        using reverse_iterator = std::vector<Entity>::reverse_iterator;

        /** @brief Add an entity. */
        auto Add(Entity::layer_type layer, Entity::flags_type flags) -> Entity
        {
            auto handle = m_handleManager.GenerateNewHandle(layer, flags);
            m_entities.push_back(handle);
            if(handle.IsPersistent())
                m_persistent.push_back(handle);

            return handle;
        }

        /** @brief Remove an entity. */
        void Remove(Entity entity)
        {
            m_toRemove.push_back(entity);
            detail::EraseUnstable(m_entities, entity);
            if (entity.IsPersistent())
                detail::EraseUnstable(m_persistent, entity);
        }

        /** @brief Check if an entity exists. */
        bool Contains(Entity entity) const
        {
            return m_entities.cend() != std::ranges::find(m_entities, entity);
        }

        /** @brief Get the number of entities in the pool. */
        auto Size() const noexcept -> size_t { return m_entities.size(); }

        /** @brief Add removed entity indices back to the pool of possible indices. */
        auto RecycleDeadEntities() -> std::vector<Entity>
        {
            if(m_toRemove.empty())
                return {};

            m_handleManager.ReclaimHandles(m_toRemove);
            return std::exchange(m_toRemove, std::vector<Entity>{});
        }

        /** @brief Remove all entities. */
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

        /** @brief Remove all entities, excluding those with the persistent flag. */
        void ClearNonPersistent()
        {
            m_toRemove.shrink_to_fit();
            m_persistent.shrink_to_fit();
            m_entities = m_persistent;
            m_handleManager.Reset(m_persistent);
        }

        /** @brief Get an iterator to the first entity in the pool. */
        auto begin() noexcept { return std::ranges::begin(m_entities); }

        /** @brief Get a const_iterator to the first entity in the pool. */
        auto begin() const noexcept { return std::ranges::begin(m_entities); }

        /** @brief Get an iterator one past the last entity in the pool. */
        auto end() noexcept { return std::ranges::end(m_entities); }

        /** @brief Get a const_iterator one past the last entity in the pool. */
        auto end() const noexcept { return std::ranges::end(m_entities); }

        /** @brief Get the number of entities in the pool. */
        auto size() const noexcept { return m_entities.size(); }

        /** @brief Check if there are no entities in the pool. */
        [[nodiscard]] auto empty() const noexcept { return m_entities.empty(); }

        /** @brief Get a reference to the entity at the specified position. */
        auto operator[](size_t pos) noexcept -> Entity& { return m_entities[pos]; }

        /** @brief Get a const reference to the entity at the specified position. */
        auto operator[](size_t pos) const noexcept -> const Entity& { return m_entities[pos]; }

        /** @brief Get a reference to the entity at the specified position with bounds checking. */
        auto at(size_t pos) -> Entity& { return m_entities.at(pos); }

        /** @brief Get a const reference to the entity at the specified position with bounds checking. */
        auto at(size_t pos) const -> const Entity& { return m_entities.at(pos); }

        /** @brief Get a pointer to the underlying entity array. */
        auto data() noexcept { return m_entities.data(); }

        /** @brief Get a pointer to the underlying entity array. */
        auto data() const noexcept { return m_entities.data(); }

    private:
        std::vector<Entity> m_entities;
        std::vector<Entity> m_toRemove;
        std::vector<Entity> m_persistent;
        detail::HandleManager m_handleManager;
};
} // namespace nc::ecs
