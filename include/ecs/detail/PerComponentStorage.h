#pragma once

#include "FreeComponentGroup.h"
#include "ecs/Component.h"
#include "debug/Utils.h"
#include "utility/Signal.h"

#include <algorithm>
#include <functional>
#include <numeric>
#include <span>
#include <vector>

namespace nc::ecs::detail
{
class PerComponentStorageBase
{
    public:
        using index_type = Entity::index_type;

        PerComponentStorageBase(size_t maxEntities)
            : sparseArray(maxEntities, Entity::NullIndex),
                entityPool{}
        {
        }

        auto SparseArray() noexcept -> std::vector<index_type>& { return sparseArray; }
        auto EntityPool() noexcept -> std::vector<Entity>& { return entityPool; }
        auto SparseArray() const noexcept -> const std::vector<index_type>& { return sparseArray; }
        auto EntityPool() const noexcept -> const std::vector<Entity>& { return entityPool; }
        auto Size() const noexcept { return entityPool.size(); }

        virtual ~PerComponentStorageBase() = default;
        virtual void Clear() = 0;
        virtual void CommitStagedComponents(const std::vector<Entity>& removed) = 0;

    protected:
        std::vector<index_type> sparseArray;
        std::vector<Entity> entityPool;
};

template<PooledComponent T>
class PerComponentStorage : public PerComponentStorageBase
{
    struct StagedComponent
    {
        Entity entity;
        T component;
    };

    public:
        using value_type = T;
        using iterator = std::vector<T>::iterator;

        PerComponentStorage(size_t maxEntities);
        ~PerComponentStorage() = default;
        PerComponentStorage(PerComponentStorage&&) = default;
        PerComponentStorage& operator=(PerComponentStorage&&) = default;
        PerComponentStorage(const PerComponentStorage&) = delete;
        PerComponentStorage& operator=(const PerComponentStorage&) = delete;

        auto ComponentPool() noexcept -> std::vector<T>& { return m_componentPool; }
        auto ComponentPool() const noexcept -> const std::vector<T>& { return m_componentPool; }

        template<class... Args>
        auto Add(Entity entity, Args&&... args) -> T*;

        void Remove(Entity entity);
        bool TryRemove(Entity entity);
        bool Contains(Entity entity) const;
        auto Get(Entity entity) -> T*;
        auto Get(Entity entity) const -> const T*;

        template<std::predicate<const T&, const T&> Predicate>
        void Sort(Predicate&& comparesLessThan);

        auto OnAdd() -> Signal<T&>&;
        auto OnRemove() -> Signal<Entity>&;

        void Swap(index_type firstEntity, index_type secondEntity);
        void ReserveHeadroom(size_t additionalRequiredCount);

        void Clear() override;
        void CommitStagedComponents(const std::vector<Entity>& removed) override;

    private:
        std::vector<T> m_componentPool;
        std::vector<StagedComponent> m_stagingPool;
        Signal<T&> m_onAdd;
        Signal<Entity> m_onRemove;
};

template<PooledComponent T>
PerComponentStorage<T>::PerComponentStorage(size_t maxEntities)
    : PerComponentStorageBase{maxEntities},
        m_componentPool{},
        m_stagingPool{},
        m_onAdd{},
        m_onRemove{}
{}

template<PooledComponent T>
template<class... Args>
auto PerComponentStorage<T>::Add(Entity entity, Args&&... args) -> T*
{
    NC_ASSERT(!Contains(entity), "Cannot add multiple components of the same type");
    auto& [emplacedEntity, emplacedComponent] = m_stagingPool.emplace_back(entity, T{entity, std::forward<Args>(args)...});

    if constexpr(StoragePolicy<T>::EnableOnAddCallbacks)
    {
        m_onAdd.Emit(emplacedComponent);
    }

    return &emplacedComponent;
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
auto PerComponentStorage<T>::OnAdd() -> Signal<T&>&
{
    return m_onAdd;
}

template<PooledComponent T>
auto PerComponentStorage<T>::OnRemove() -> Signal<Entity>&
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
