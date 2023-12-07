/**
 * @file ComponentPool.h
 * @copyright Jaremie Romer and McCallister Romer 2023
 */
#pragma once

#include "ncengine/ecs/AnyComponent.h"
#include "ncengine/ecs/detail/PoolUtility.h"
#include "ncengine/ecs/detail/SparseSet.h"
#include "ncengine/utility/Signal.h"

#include "ncutility/NcError.h"

#include <cassert>
#include <string_view>

namespace nc::ecs
{
/**
 * @brief Generic interface for component pools.
 * 
 * ComponentPoolBase is the type-agnostic interface for a component pool.
 * 
*/
class ComponentPoolBase
{
    public:
        using entity_iterator = std::span<const Entity>::iterator;

        /** @brief Check if the pool has a component attached to an entity. */
        virtual auto Contains(Entity entity) const -> bool = 0;

        /** @brief Get the component attached to an entity as an AnyComponent.
         *  @note Returns a null object if no component exists. */
        virtual auto GetAsAnyComponent(Entity entity) -> AnyComponent = 0;

        /** @brief Remove the component attached to an entity */
        virtual auto Remove(Entity entity) -> bool = 0;

        /** @brief Get the number of components in the pool, excluding those still staged. */
        virtual auto Size() const noexcept -> size_t = 0;

        /** @brief Get the number of staged components waiting to be merged into the pool. */
        virtual auto StagedSize() const noexcept -> size_t = 0;

        /** @brief Get the combined number of components in the pool and staged. */
        virtual auto TotalSize() const noexcept -> size_t = 0;

        /** @brief Pre-allocate space for some number of components. */
        virtual void Reserve(size_t capacity) = 0;

        /** @brief Get a span containing the entities with components in the pool. */
        virtual auto GetEntityPool() const noexcept -> std::span<const Entity> = 0;

        /** @brief Get the name from the component's ComponentHandler. */
        virtual auto GetComponentName() const noexcept -> std::string_view = 0;

        /** @brief Check if there is a drawUI callback set in the the component's ComponentHandler. */
        virtual auto HasDrawUI() const noexcept -> bool = 0;

        /** @brief Remove all components not attached to a persistent entity.
         *  @note This operation is handled automatically for pools owned by the ComponentRegistry. */
        virtual void ClearNonPersistent() = 0;

        /** @brief Remove all components.
         *  @note This operation is handled automatically for pools owned by the ComponentRegistry. */
        virtual void Clear() = 0;

        /** @brief Finalize pending changes by merging staged components and removing data for any
         *         entities deleted from the ComponentRegistry.
         *  @note This operation is handled automatically for pools owned by the ComponentRegistry. */
        virtual void CommitStagedComponents(const std::vector<Entity>& deleted) = 0;
};

/**
 * @brief .
 * @tparam T 
 * 
 * 
 */
template<PooledComponent T>
class ComponentPool final : public ComponentPoolBase,
                            public std::ranges::view_interface<ComponentPool<T>>
{
    public:
        using value_type = T;
        using iterator = std::vector<T>::iterator;
        using const_iterator = std::vector<T>::const_iterator;
        using reverse_iterator = std::vector<T>::reverse_iterator;

        /** @brief  */
        explicit ComponentPool(size_t maxEntities, ComponentHandler<T> handler)
            : m_storage{maxEntities}, m_handler{std::move(handler)} {}

        ~ComponentPool() = default;
        ComponentPool(ComponentPool&&) = default;
        ComponentPool& operator=(ComponentPool&&) = default;
        ComponentPool(const ComponentPool&) = delete;
        ComponentPool& operator=(const ComponentPool&) = delete;

        /** @brief Emplace a component attached to an entity. */
        template<class... Args>
        auto Add(Entity entity, Args&&... args) -> T*;

        /** @brief Get the component attached to an entity, or nullptr on failure. */
        auto Get(Entity entity) -> T*;
        auto Get(Entity entity) const -> const T*;

        /** @brief Get the entity a component is attached to, or a null entity on failure. */
        auto GetParent(const T* component) const -> Entity;

        /** @brief View all components as a contiguous range. */
        auto GetComponents() noexcept -> std::span<T> { return m_storage.GetPackedArray(); }

        /** @brief View all components as a contiguous range. */
        auto GetComponents() const noexcept -> std::span<const T> { return m_storage.GetPackedArray(); }

        /** @brief Get the T's ComponentHandler. */
        auto Handler() noexcept -> ComponentHandler<T>& { return m_handler; }

        /** @brief Get the T's onAdd event Signal. */
        auto OnAdd() noexcept -> Signal<T&>& { return m_onAdd; }

        /** @brief Get the T's onRemove event Signal. */
        auto OnRemove() noexcept -> Signal<Entity>& { return m_onRemove; }

        /** @brief Sort the components according to a predicate. */
        template<std::predicate<const T&, const T&> Predicate>
        void Sort(Predicate&& pred);

        // TODO: sort out documentation for these - should be grabbed by base class
        //       is including duplicates

        auto Contains(Entity entity) const -> bool override;
        auto GetAsAnyComponent(Entity entity) -> AnyComponent override;
        auto Remove(Entity entity) -> bool override;
        auto Size() const noexcept -> size_t override { return m_storage.Size(); }
        auto StagedSize() const noexcept -> size_t override { return m_staging.size(); }
        auto TotalSize() const noexcept -> size_t override { return Size() + StagedSize(); }
        void Reserve(size_t capacity) override;
        auto GetEntityPool() const noexcept -> std::span<const Entity> override { return m_storage.GetEntities(); }
        auto GetComponentName() const noexcept -> std::string_view override { return m_handler.name; }
        auto HasDrawUI() const noexcept -> bool override { return m_handler.drawUI != nullptr; }
        void ClearNonPersistent() override;
        void Clear() override;

        // span?
        void CommitStagedComponents(const std::vector<Entity>& deleted) override;

        // TODO: document funcs enabled by view_interface
        auto begin() noexcept { return std::ranges::begin(m_storage.GetPackedArray()); }
        auto begin() const noexcept { return std::ranges::begin(m_storage.GetPackedArray()); }
        auto end() noexcept { return std::ranges::end(m_storage.GetPackedArray()); }
        auto end() const noexcept { return std::ranges::end(m_storage.GetPackedArray()); }

    private:
        ecs::detail::SparseSet<T> m_storage;
        std::vector<detail::StagedComponent<T>> m_staging;
        ComponentHandler<T> m_handler;
        Signal<T&> m_onAdd;
        Signal<Entity> m_onRemove;
};

/** @cond internal */
template<PooledComponent T>
template<class... Args>
auto ComponentPool<T>::Add(Entity entity, Args&&... args) -> T*
{
    NC_ASSERT(entity.Index() < m_storage.MaxSize() && !Contains(entity), "Bad entity");
    auto& [_, component] = m_staging.emplace_back(
        entity,
        detail::Construct<T>(entity, std::forward<Args>(args)...)
    );

    if constexpr(StoragePolicy<T>::EnableOnAddCallbacks)
        m_onAdd.Emit(component);

    return &component;
}

template<PooledComponent T>
auto ComponentPool<T>::Remove(Entity entity) -> bool
{
    NC_ASSERT(entity.Valid(), "Bad entity");
    if (!m_storage.Remove(entity) && !detail::EraseUnstable(m_staging, entity))
        return false;

    if constexpr(StoragePolicy<T>::EnableOnRemoveCallbacks)
        m_onRemove.Emit(entity);

    return true;
}

template<PooledComponent T>
bool ComponentPool<T>::Contains(Entity entity) const
{
    return m_storage.Contains(entity)
        ? true
        : std::cend(m_staging) != std::ranges::find(
            m_staging, entity, &detail::StagedComponent<T>::entity);
}

template<PooledComponent T>
auto ComponentPool<T>::Get(Entity entity) -> T*
{
    NC_ASSERT(entity.Valid(), "Bad entity");
    if (m_storage.Contains(entity))
        return &m_storage.Get(entity);

    auto pos = std::ranges::find(m_staging, entity, &detail::StagedComponent<T>::entity);
    return pos != std::cend(m_staging) ? &pos->component : nullptr;
}

template<PooledComponent T>
auto ComponentPool<T>::Get(Entity entity) const -> const T*
{
    NC_ASSERT(entity.Valid(), "Bad entity");
    if (m_storage.Contains(entity))
        return &m_storage.Get(entity);

    auto pos = std::ranges::find(m_staging, entity);
    return pos != std::cend(m_staging) ? &pos->component : nullptr;
}

template<PooledComponent T>
auto ComponentPool<T>::GetAsAnyComponent(Entity entity) -> AnyComponent
{
    return Contains(entity) ? AnyComponent{Get(entity), &m_handler} : AnyComponent{};
}

template<PooledComponent T>
auto ComponentPool<T>::GetParent(const T* component) const -> Entity
{
    if (auto parent = m_storage.GetParent(component); parent.Valid())
        return parent;

    const auto pos = std::ranges::find(m_staging, component, [](auto&& staged) { return &staged.component; });
    return pos != std::cend(m_staging) ? pos->entity : Entity::Null();
}

template<PooledComponent T>
template<std::predicate<const T&, const T&> Pred>
void ComponentPool<T>::Sort(Pred&& lessThan)
{
    m_storage.Sort(std::forward<Pred>(lessThan));
}

template<PooledComponent T>
void ComponentPool<T>::Reserve(size_t capacity)
{
    m_storage.Reserve(capacity);
    const auto existing = TotalSize();
    if (capacity > existing)
    {
        // We don't need to reserve the whole capacity for staging - we just
        // want to guarantee ptrs remain valid until we exceed capacity.
        m_staging.reserve(capacity - existing);
    }
}

template<PooledComponent T>
void ComponentPool<T>::CommitStagedComponents(const std::vector<Entity>& deleted)
{
    for(auto entity : deleted)
        Remove(entity);

    for(auto& [entity, component] : m_staging)
        m_storage.Insert(entity, std::move(component));

    m_staging.clear();
}

template<PooledComponent T>
void ComponentPool<T>::ClearNonPersistent()
{
    assert(m_staging.empty());
    m_staging.shrink_to_fit();
    m_storage.ClearNonPersistent();
}

template<PooledComponent T>
void ComponentPool<T>::Clear()
{
    m_staging.clear();
    m_staging.shrink_to_fit();
    m_storage.Clear();
}
/** @endcond */
} // namespace nc::ecs
