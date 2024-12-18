/**
 * @file FreeComponentPool.h
 * @copyright Jaremie Romer and McCallister Romer 2024
 */
#pragma once

#include "ncengine/ecs/detail/FreeComponentGroup.h"
#include "ncengine/utility/SparseMap.h"

#include <ranges>

namespace nc::ecs
{
/** @brief Single pool storing all FreeComponent instances. */
class FreeComponentPool
{
    public:
        explicit FreeComponentPool(size_t maxEntities)
            : m_entityToGroup{0, maxEntities}
        {
        }

        /** @brief Construct an instance of T and attach it to an Entity. */
        template<std::derived_from<FreeComponent> T, class ... Args>
        auto Emplace(Entity entity, Args&& ... args) -> T&
        {
            const auto id = entity.Index();
            auto& group = m_entityToGroup.contains(id)
                ? m_entityToGroup.at(id)
                : m_entityToGroup.emplace(id, detail::FreeComponentGroup{});

            return group.Add<T>(entity, std::forward<Args>(args)...);
        }

        /** @brief Remove the instance of T from an Entity. */
        template<std::derived_from<FreeComponent> T>
        auto Remove(Entity entity) -> bool
        {
            const auto id = entity.Index();
            return m_entityToGroup.contains(id)
                ? m_entityToGroup.at(entity.Index()).Remove<T>()
                : false;
        }

        /** @brief Check if there is an instance of T attached to an Entity. */
        template<std::derived_from<FreeComponent> T>
        bool Contains(Entity entity) const noexcept
        {
            const auto id = entity.Index();
            return m_entityToGroup.contains(id)
                ? m_entityToGroup.at(id).Contains<T>()
                : false;
        }

        /** @brief Get the instance of T attached to an Entity. */
        template<std::derived_from<FreeComponent> T>
        auto Get(Entity entity) const -> T&
        {
            NC_ASSERT(m_entityToGroup.contains(entity.Index()), "Component does not exist");
            return m_entityToGroup.at(entity.Index()).Get<T>();
        }

        /** @brief Finalize any staged additions and removals. */
        void CommitStagedComponents(std::span<const Entity> removedEntities)
        {
            for (const auto removed : removedEntities)
            {
                m_entityToGroup.erase(removed.Index());
            }

            for (auto& group : m_entityToGroup.values())
            {
                group.CommitStagedComponents();
            }
        }

        /** @brief Remove all components not attached to persistent Entities. */
        void ClearNonPersistent() noexcept
        {
            const auto keyVals = std::views::zip(m_entityToGroup.keys(), m_entityToGroup.values());
            for (const auto [id, group] : std::views::reverse(keyVals))
            {
                if (!group.IsPersistentGroup())
                {
                    m_entityToGroup.erase(id);
                }
            }
        }

        /** @brief Remove all components. */
        void Clear() noexcept
        {
            m_entityToGroup.clear();
        }

    private:
        sparse_map<detail::FreeComponentGroup> m_entityToGroup;
};
} // namespace nc::ecs
