#pragma once

#include "AnyComponent.h"
#include "Entity.h"

#include <span>
#include <string_view>
#include <vector>

namespace nc::ecs
{
class ComponentPool
{
    public:
        using index_type = Entity::index_type;

        ComponentPool(size_t maxEntities)
            : sparseArray(maxEntities, Entity::NullIndex), entityPool{} {}

        virtual ~ComponentPool() = default;

        /** @brief Get the value of `ComponentHandler::name` for the underlying type. */
        virtual auto GetComponentName() const noexcept -> std::string_view = 0;

        /** @brief Check if `ComponentHandler::factory` is set for the underlying type. */
        virtual auto HasFactory() const noexcept -> bool = 0;

        /** @brief Factory-construct a component attached to an entity. */
        virtual auto AddDefault(Entity entity) -> AnyComponent = 0;

        /** @brief Check if a component is attached to an entity. */
        virtual bool Contains(Entity entity) const = 0;

        /** @brief Get the component attached to an entity, or a null AnyComponent if none exists. */
        virtual auto GetAsAnyComponent(Entity entity) -> AnyComponent = 0;

        /** @brief Remove a component attached to an entity if one exists. */
        virtual bool TryRemove(Entity entity) = 0;

        /** @brief Remove all components except those attached to persistent entities.
         *  @note This operation is managed automatically for pools owned by the `Registry`. */
        virtual void Clear() = 0;

        /** @brief Merge any staged changes into the internal pool.
         *  @note This operation is managed automatically for pools owned by the `Registry`. */
        virtual void CommitStagedComponents(const std::vector<Entity>& removed) = 0;

        /** @brief Get a collection of all entities with attached components in the pool. */
        auto EntityPool() noexcept -> std::span<Entity>
        {
            return entityPool;
        }

        /** @brief Get the number of components in the pool. */
        auto Size() const noexcept -> size_t
        {
            return entityPool.size();
        }

    protected:
        std::vector<index_type> sparseArray;
        std::vector<Entity> entityPool;
};
} // namespace ecs
