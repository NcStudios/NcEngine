#pragma once

#include "FreeComponentGroup.h"
#include "ncengine/ecs/Component.h"
#include "ncengine/ecs/ComponentPool.h"
#include "ncengine/utility/Signal.h"

#include <algorithm>
#include <cassert>
#include <functional>
#include <numeric>

namespace nc::ecs::detail
{
template<class T, class... Args>
auto ConstructComponent(Entity entity, Args&&... args)
{
    if constexpr (std::constructible_from<T, Entity, Args...>)
    {
        return T{entity, std::forward<Args>(args)...};
    }
    else
    {
        return T{std::forward<Args>(args)...};
    }
}

template<PooledComponent T>
class PerComponentStorage final : public ComponentPool
{
    struct StagedComponent
    {
        Entity entity;
        T component;
    };

    public:
        using value_type = T;
        using iterator = std::vector<T>::iterator;

        PerComponentStorage(size_t maxEntities, ComponentHandler<T> handler);

        template<class... Args>
        auto Add(Entity entity, Args&&... args) -> T*;
        void Remove(Entity entity);
        auto Get(Entity entity) -> T*;
        auto Get(Entity entity) const -> const T*;
        auto GetParent(const T* component) const -> Entity;

        template<std::predicate<const T&, const T&> Predicate>
        void Sort(Predicate&& comparesLessThan);
        void Swap(index_type firstEntity, index_type secondEntity);
        void ReserveHeadroom(size_t additionalRequiredCount);
        auto GetComponents() noexcept -> std::vector<T>& { return m_componentPool; }
        auto GetComponents() const noexcept -> const std::vector<T>& { return m_componentPool; }

        auto OnAdd() noexcept -> Signal<T&>&;
        auto OnRemove() noexcept -> Signal<Entity>&;
        auto Handler() noexcept -> ComponentHandler<T>&;

        auto GetComponentName() const noexcept -> std::string_view override;
        auto HasFactory() const noexcept -> bool override;
        auto HasDrawUI() const noexcept -> bool override;
        auto HasUserData() const noexcept -> bool override;
        auto Contains(Entity entity) const -> bool override;
        auto GetAsAnyComponent(Entity entity) -> AnyComponent override;
        auto AddDefault(Entity entity) -> AnyComponent override;
        auto TryRemove(Entity entity) -> bool override;
        void Clear() override;
        void CommitStagedComponents(const std::vector<Entity>& removed) override;

    private:
        std::vector<T> m_componentPool;
        std::vector<StagedComponent> m_stagingPool;
        ComponentHandler<T> m_handler;
        Signal<T&> m_onAdd;
        Signal<Entity> m_onRemove;
};

template<PooledComponent T>
PerComponentStorage<T>::PerComponentStorage(size_t maxEntities, ComponentHandler<T> handler)
    : ecs::ComponentPool{maxEntities},
      m_componentPool{},
      m_stagingPool{},
      m_handler{std::move(handler)},
      m_onAdd{},
      m_onRemove{}
{}

template<PooledComponent T>
template<class... Args>
auto PerComponentStorage<T>::Add(Entity entity, Args&&... args) -> T*
{
    NC_ASSERT(!Contains(entity), "Cannot add multiple components of the same type");
    auto& [_, component] = m_stagingPool.emplace_back(entity, ConstructComponent<T>(entity, std::forward<Args>(args)...));

    if constexpr(StoragePolicy<T>::EnableOnAddCallbacks)
    {
        m_onAdd.Emit(component);
    }

    return &component;
}

template<PooledComponent T>
void PerComponentStorage<T>::Remove(Entity entity)
{
    auto sparseIndex = entity.Index();
    auto poolIndex = sparseArray.at(sparseIndex);

    NC_ASSERT(poolIndex != Entity::NullIndex, "Entity does not have component");

    m_componentPool.at(poolIndex) = std::move(m_componentPool.back());
    m_componentPool.pop_back();

    auto movedEntity = entityPool.back(); // need to store in case we're removing the last element
    entityPool.at(poolIndex) = movedEntity;
    entityPool.pop_back();

    sparseArray.at(sparseIndex) = Entity::NullIndex;

    if(sparseIndex != movedEntity.Index())
        sparseArray.at(movedEntity.Index()) = poolIndex;

    if constexpr(StoragePolicy<T>::EnableOnRemoveCallbacks)
    {
        m_onRemove.Emit(entity);
    }
}

template<PooledComponent T>
bool PerComponentStorage<T>::TryRemove(Entity entity)
{
    if(!Contains(entity))
        return false;

    Remove(entity);
    return true;
}

template<PooledComponent T>
bool PerComponentStorage<T>::Contains(Entity entity) const
{
    if(sparseArray.at(entity.Index()) != Entity::NullIndex)
        return true;

    return m_stagingPool.cend() != std::ranges::find_if(m_stagingPool, [entity](const auto& pair)
    {
        return entity == pair.entity;
    });
}

template<PooledComponent T>
auto PerComponentStorage<T>::Get(Entity entity) -> T*
{
    auto poolIndex = sparseArray.at(entity.Index());
    if(poolIndex != Entity::NullIndex)
        return &m_componentPool.at(poolIndex);

    auto pos = std::ranges::find_if(m_stagingPool, [entity](const auto& pair)
    {
        return pair.entity == entity;
    });

    return pos == m_stagingPool.end() ? nullptr : &pos->component;
}

template<PooledComponent T>
auto PerComponentStorage<T>::Get(Entity entity) const -> const T*
{
    auto poolIndex = sparseArray.at(entity.Index());
    if(poolIndex != Entity::NullIndex)
        return &m_componentPool.at(poolIndex);

    const auto pos = std::ranges::find_if(m_stagingPool, [entity](const auto& pair)
    {
        return pair.entity == entity;
    });

    return pos == m_stagingPool.end() ? nullptr : &pos->component;
}

template<PooledComponent T>
auto PerComponentStorage<T>::GetAsAnyComponent(Entity entity) -> AnyComponent
{
    return Contains(entity) ?
        AnyComponent{Get(entity), &m_handler} :
        AnyComponent{};
}

template<PooledComponent T>
auto PerComponentStorage<T>::GetParent(const T* component) const -> Entity
{
    const auto beg = m_componentPool.data();
    const auto end = beg + m_componentPool.size();
    if (component >= beg && component < end)
    {
        return entityPool.at(static_cast<size_t>(component - beg));
    }

    const auto pos = std::ranges::find_if(m_stagingPool, [component](const auto& staged)
    {
        return &staged.component == component;
    });

    return pos != m_stagingPool.cend() ? pos->entity : Entity::Null();
}

template<PooledComponent T>
template<std::predicate<const T&, const T&> Predicate>
void PerComponentStorage<T>::Sort(Predicate&& comparesLessThan)
{
    /** Create array of indices for an out-of-place sort. */
    const auto size = m_componentPool.size();
    std::vector<uint32_t> permutation(size);
    const auto beg = permutation.begin();
    const auto end = permutation.end();
    std::iota(beg, end, 0u);

    /** Extend the user-provided predicate to operate in terms of indices. */
    auto compare = [&pool = m_componentPool, predicate = std::forward<Predicate>(comparesLessThan)](const auto lhs, const auto rhs)
    {
        return predicate(pool[lhs], pool[rhs]);
    };

    /** Insertion sort - @todo radix sort used in physics is probably very fast here. */
    for(auto cur = beg; cur != end; ++cur)
    {
        const auto pos = std::upper_bound(beg, cur, *cur, compare);
        std::rotate(pos, cur, cur + 1);
    }

    /** Apply the permutation. */
    for(auto i = 0u; i < size; ++i)
    {
        auto cur = i;
        auto next = permutation[cur];

        while(cur != next)
        {
            const auto i1 = permutation[cur];
            const auto i2 = permutation[next];
            std::swap(entityPool[i1], entityPool[i2]);
            std::swap(m_componentPool[i1], m_componentPool[i2]);
            const auto sparse1 = entityPool[i1].Index();
            const auto sparse2 = entityPool[i2].Index();
            sparseArray[sparse1] = i2;
            sparseArray[sparse2] = i1;
            permutation[cur] = cur;
            cur = next;
            next = permutation[cur];
        }
    }
}

template<PooledComponent T>
auto PerComponentStorage<T>::Handler() noexcept -> ComponentHandler<T>&
{
    return m_handler;
}

template<PooledComponent T>
auto PerComponentStorage<T>::OnAdd() noexcept -> Signal<T&>&
{
    return m_onAdd;
}

template<PooledComponent T>
auto PerComponentStorage<T>::OnRemove() noexcept -> Signal<Entity>&
{
    return m_onRemove;
}

template<PooledComponent T>
void PerComponentStorage<T>::Swap(index_type firstEntity, index_type secondEntity)
{
    auto firstDenseIndex = sparseArray.at(firstEntity);
    auto secondDenseIndex = sparseArray.at(secondEntity);
    std::swap(entityPool.at(firstDenseIndex), entityPool.at(secondDenseIndex));
    std::swap(m_componentPool.at(firstDenseIndex), m_componentPool.at(secondDenseIndex));
    sparseArray.at(firstEntity) = secondDenseIndex;
    sparseArray.at(secondEntity) = firstDenseIndex;
}

template<PooledComponent T>
void PerComponentStorage<T>::ReserveHeadroom(size_t additionalRequiredCount)
{
    auto requiredSize = m_componentPool.size() + additionalRequiredCount;
    m_componentPool.reserve(requiredSize);
    entityPool.reserve(requiredSize);
}

template<PooledComponent T>
auto PerComponentStorage<T>::GetComponentName() const noexcept -> std::string_view
{
    return m_handler.name;
}

template<PooledComponent T>
auto PerComponentStorage<T>::HasFactory() const noexcept -> bool
{
    return m_handler.factory != nullptr;
}

template<PooledComponent T>
auto PerComponentStorage<T>::HasUserData() const noexcept -> bool
{
    return m_handler.userData != nullptr;
}

template<PooledComponent T>
auto PerComponentStorage<T>::HasDrawUI() const noexcept -> bool
{
    return m_handler.drawUI != nullptr;
}

template<PooledComponent T>
auto PerComponentStorage<T>::AddDefault(Entity entity) -> AnyComponent
{
    if (m_handler.factory)
    {
        auto comp = Add(entity, m_handler.factory(entity, m_handler.userData));
        return AnyComponent{comp, &m_handler};
    }

    return AnyComponent{};
}

template<PooledComponent T>
void PerComponentStorage<T>::CommitStagedComponents(const std::vector<Entity>& removed)
{
    for(auto entity : removed)
        TryRemove(entity);

    for(auto& [entity, component] : m_stagingPool)
    {
        m_componentPool.push_back(std::move(component));
        auto sparseIndex = entity.Index();
        entityPool.push_back(entity);
        sparseArray.at(sparseIndex) = static_cast<uint32_t>(m_componentPool.size() - 1);
    }

    m_stagingPool.clear();

    if constexpr(std::same_as<T, FreeComponentGroup>)
    {
        for(auto& group : m_componentPool)
        {
            group.CommitStagedComponents();
        }
    }
}

template<PooledComponent T>
void PerComponentStorage<T>::Clear()
{
    assert(m_stagingPool.empty());
    m_stagingPool.shrink_to_fit();

    std::ranges::fill(sparseArray, Entity::NullIndex);

    uint32_t swapToIndex = 0u;
    for(uint32_t denseIndex = 0u; denseIndex < entityPool.size(); ++denseIndex)
    {
        auto entity = entityPool[denseIndex];
        if(!entity.IsPersistent())
            continue;

        sparseArray.at(entity.Index()) = swapToIndex;
        entityPool.at(swapToIndex) = entityPool.at(denseIndex);
        m_componentPool.at(swapToIndex) = std::move(m_componentPool.at(denseIndex));
        ++swapToIndex;
    }

    entityPool.erase(entityPool.begin() + swapToIndex, entityPool.end());
    entityPool.shrink_to_fit();
    m_componentPool.erase(m_componentPool.begin() + swapToIndex, m_componentPool.end());
    m_componentPool.shrink_to_fit();
}
} // namespace nc::ecs::detail
