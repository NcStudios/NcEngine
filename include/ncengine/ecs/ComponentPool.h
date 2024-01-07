/**
 * @file ComponentPool.h
 * @copyright Jaremie Romer and McCallister Romer 2024
 */
#pragma once

#include "ncengine/ecs/AnyComponent.h"
#include "ncengine/ecs/detail/PoolUtility.h"
#include "ncengine/ecs/detail/SparseSet.h"
#include "ncengine/type/StableAddress.h"
#include "ncengine/utility/Signal.h"

#include "ncutility/NcError.h"

#include <cassert>
#include <string_view>

namespace nc::ecs
{
/** @brief Type-agnostic base class for component pools. */
class ComponentPoolBase : public StableAddress
{
    public:
        using entity_iterator = std::span<const Entity>::iterator;

        virtual ~ComponentPoolBase() = default;

        /** @brief Get the component's unique id. */
        virtual auto Id() const noexcept -> size_t = 0;

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

        /** @brief Check if the factory callback is set in the component's ComponentHandler. */
        virtual auto HasFactory() const noexcept -> bool = 0;

        /** @brief Check if userData is set in the component's ComponentHandler. */
        virtual auto HasUserData() const noexcept -> bool = 0;

        /** @brief Check if the serialize and deserialize callbacks are set in the component's ComponentHandler. */
        virtual auto HasSerialize() const noexcept -> bool = 0;

        /** @brief Check if the drawUI callback set in the the component's ComponentHandler. */
        virtual auto HasDrawUI() const noexcept -> bool = 0;

        /**
         * @brief Factory-construct a component attached to an entity.
         * @note Returns a null AnyComponent if there is no registered factory handler.
         */
        virtual auto AddDefault(Entity entity) -> AnyComponent = 0;

        /** @brief Serialize the component attached to an entity to a binary stream. */
        virtual void Serialize(std::ostream& stream, Entity entity, const SerializationContext& ctx) = 0;

        /** @brief Deserialize a component from a binary stream and attach it to an entity. */
        virtual void Deserialize(std::istream& stream, Entity entity, const DeserializationContext& ctx) = 0;

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

/** @brief Type-aware implementation for component pools. */
template<PooledComponent T>
class ComponentPool final : public ComponentPoolBase
{
    public:
        using value_type = T;
        using iterator = std::vector<T>::iterator;
        using const_iterator = std::vector<T>::const_iterator;
        using reverse_iterator = std::vector<T>::reverse_iterator;

        /** @brief Construct a new component pool. */
        explicit ComponentPool(size_t maxEntities, ComponentHandler<T> handler)
            : m_storage{maxEntities}, m_handler{std::move(handler)} {}

        /** @brief Emplace a component attached to an entity. */
        template<class... Args>
        auto Add(Entity entity, Args&&... args) -> T*;

        /** @brief Insert a component attached to an entity. */
        auto Insert(Entity entity, T obj) -> T*;

        /** @brief Get a pointer to the component attached to an entity, or nullptr if one does not exist. */
        auto Get(Entity entity) -> T*;

        /** @brief Get a const pointer to the component attached to an entity, or nullptr if one does not exist. */
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
        void Sort(Predicate&& compare);

        /** @brief Get an iterator the the first component in the pool. */
        auto begin() noexcept { return std::ranges::begin(m_storage.GetPackedArray()); }

        /** @brief Get a const_iterator the the first component in the pool. */
        auto begin() const noexcept { return std::ranges::begin(m_storage.GetPackedArray()); }

        /** @brief Get an iterator one past the last component in the pool. */
        auto end() noexcept { return std::ranges::end(m_storage.GetPackedArray()); }

        /** @brief Get a const_iterator one past the last component in the pool. */
        auto end() const noexcept { return std::ranges::end(m_storage.GetPackedArray()); }

        /** @brief Get the number of components committed to the pool. */
        auto size() const noexcept { return Size(); }

        /** @brief Check if there are no components in the pool. */
        [[nodiscard]] auto empty() const noexcept { return GetComponents().empty(); }

        /** @brief Get a reference to the component at the specified position. */
        auto operator[](size_t pos) noexcept -> T& { return GetComponents()[pos]; }

        /** @brief Get a const reference to the component at the specified position. */
        auto operator[](size_t pos) const noexcept -> const T& { return GetComponents()[pos]; }

        /** @brief Get a reference to the component at the specified position with bounds checking. */
        auto at(size_t pos) -> T& { return GetComponents().at(pos); }

        /** @brief Get a reference to the component at the specified position with bounds checking. */
        auto at(size_t pos) const -> const T& { return GetComponents().at(pos); }

        /** @brief Get a pointer to the underlying component array. */
        auto data() noexcept { return GetComponents().data(); }

        /** @brief Get a pointer to the underlying component array. */
        auto data() const noexcept { return GetComponents().data(); }

        auto Contains(Entity entity) const -> bool override;
        auto GetAsAnyComponent(Entity entity) -> AnyComponent override;
        auto Remove(Entity entity) -> bool override;
        auto Size() const noexcept -> size_t override { return m_storage.Size(); }
        auto StagedSize() const noexcept -> size_t override { return m_staging.size(); }
        auto TotalSize() const noexcept -> size_t override { return Size() + StagedSize(); }
        void Reserve(size_t capacity) override;
        auto GetEntityPool() const noexcept -> std::span<const Entity> override { return m_storage.GetEntities(); }
        auto GetComponentName() const noexcept -> std::string_view override { return m_handler.name; }
        auto HasFactory() const noexcept -> bool override { return m_handler.factory != nullptr; }
        auto HasUserData() const noexcept -> bool override { return m_handler.userData != nullptr; }
        auto HasSerialize() const noexcept -> bool override { return m_handler.serialize && m_handler.deserialize; }
        auto HasDrawUI() const noexcept -> bool override { return m_handler.drawUI != nullptr; }
        auto Id() const noexcept -> size_t override { return m_handler.id; };
        auto AddDefault(Entity entity) -> AnyComponent override;
        void Serialize(std::ostream& stream, Entity entity, const SerializationContext& ctx) override;
        void Deserialize(std::istream& stream, Entity entity, const DeserializationContext& ctx) override;
        void ClearNonPersistent() override;
        void Clear() override;
        void CommitStagedComponents(const std::vector<Entity>& deleted) override;

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
auto ComponentPool<T>::Insert(Entity entity, T obj) -> T*
{
    NC_ASSERT(entity.Index() < m_storage.MaxSize() && !Contains(entity), "Bad entity");
    auto& [_, component] = m_staging.emplace_back(entity, std::move(obj));

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
        : std::cend(m_staging) != std::ranges::find(m_staging, entity, &detail::StagedComponent<T>::entity);
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
void ComponentPool<T>::Sort(Pred&& compare)
{
    m_storage.Sort(std::forward<Pred>(compare));
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
auto ComponentPool<T>::AddDefault(Entity entity) -> AnyComponent
{
    if (m_handler.factory)
    {
        auto comp = Add(entity, m_handler.factory(entity, m_handler.userData));
        return AnyComponent{comp, &m_handler};
    }

    return AnyComponent{};
}

template<PooledComponent T>
void ComponentPool<T>::Serialize(std::ostream& stream, Entity entity, const SerializationContext& ctx)
{
    NC_ASSERT(HasSerialize() && Contains(entity), "...");
    m_handler.serialize(stream, *Get(entity), ctx, m_handler.userData);

}

template<PooledComponent T>
void ComponentPool<T>::Deserialize(std::istream& stream, Entity entity, const DeserializationContext& ctx)
{
    NC_ASSERT(HasSerialize(), "...");
    Insert(entity, m_handler.deserialize(stream, ctx, m_handler.userData));
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
