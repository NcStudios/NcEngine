#pragma once

#include "ncengine/ecs/Component.h"

#include "ncutility/Algorithm.h"

#include <algorithm>
#include <numeric>
#include <span>
#include <utility>
#include <vector>

/** @cond internal */
namespace nc::ecs::detail
{
/**
 * Associative container used for entity-component storage.
 *
 * Maintains a packed array of components and a sparse array of indices into the packed array. This
 * provides both a contiguous view of all components and constant time random lookup of a component,
 * given its parent entity, at the cost of space + extra indirection.
 *
 * The sparse array is allocated upon construction using capacity * 4 bytes, while the component
 * array grows dynamically up to this size. An additional array of entities is kept, with the same
 * order as the components. This enables look up from a component to an entity and viewing a
 * component pool as a range of entities.
 *
 * Random access is achieved by indexing the sparse array with an entity's index, then indexing
 * into the packed array with the resulting index to get the component. A value of
 * Entity::NullIndex in the sparse array indicates no component is attached to that entity.
 */
template<class T>
class SparseSet
{
    public:
        using index_type = Entity::index_type;

        explicit SparseSet(size_t maxCount)
            : m_sparse(maxCount, Entity::NullIndex) {}

        auto Insert(Entity entity, const T& component) -> T&;
        auto Insert(Entity entity, T&& component) -> T&;
        auto Remove(Entity entity) -> bool;
        auto Get(Entity entity) -> T&;
        auto Get(Entity entity) const -> const T&;
        auto GetParent(const T* component) const noexcept -> Entity;
        auto GetIndex(Entity entity) const -> index_type;
        auto GetEntity(index_type index) const -> Entity;
        auto Contains(Entity entity) const noexcept -> bool;
        auto Size() const noexcept -> size_t { return m_dense.size(); }
        auto MaxSize() const noexcept -> size_t { return m_sparse.size(); }
        void Swap(Entity lhs, Entity rhs);
        void Reserve(size_t capacity);
        auto GetPackedArray() noexcept -> std::span<T> { return m_dense; }
        auto GetPackedArray() const noexcept -> std::span<const T> { return m_dense; }
        auto GetEntities() const noexcept -> std::span<const Entity> { return m_entities; }
        void ClearNonPersistent();
        void Clear();

        template<std::predicate<const T&, const T&> Predicate>
        void Sort(Predicate&& lessThan);

    private:
        std::vector<index_type> m_sparse;
        std::vector<T> m_dense;
        std::vector<Entity> m_entities;

        void InsertEntity(Entity entity);
};

template<class T>
void SparseSet<T>::InsertEntity(Entity entity)
{
    auto& denseIndex = m_sparse.at(entity.Index());
    if (denseIndex != Entity::NullIndex)
        throw NcError{"Entity already exists."};

    denseIndex = static_cast<index_type>(m_entities.size());
    m_entities.push_back(entity);
}

template<class T>
auto SparseSet<T>::Insert(Entity entity, const T& component) -> T&
{
    InsertEntity(entity);
    m_dense.push_back(component);
    return m_dense.back();
}

template<class T>
auto SparseSet<T>::Insert(Entity entity, T&& component) -> T&
{
    InsertEntity(entity);
    m_dense.push_back(std::move(component));
    return m_dense.back();
}

template<class T>
auto SparseSet<T>::Remove(Entity toRemove) -> bool
{
    if (!Contains(toRemove))
        return false;

    const auto toRemoveSparse = toRemove.Index();
    const auto swappedSparse = m_entities.back().Index();
    const auto toRemoveDense = m_sparse.at(toRemoveSparse);

    m_dense.at(toRemoveDense) = std::move(m_dense.back());
    m_dense.pop_back();
    m_entities.at(toRemoveDense) = m_entities.back();
    m_entities.pop_back();
    m_sparse.at(swappedSparse) = toRemoveDense;
    m_sparse.at(toRemoveSparse) = Entity::NullIndex;
    return true;
}

template<class T>
auto SparseSet<T>::Get(Entity entity) -> T&
{
    return m_dense.at(m_sparse.at(entity.Index()));
}

template<class T>
auto SparseSet<T>::Get(Entity entity) const -> const T&
{
    return m_dense.at(m_sparse.at(entity.Index()));
}

template<class T>
auto SparseSet<T>::GetParent(const T* component) const noexcept -> Entity
{
    const auto beg = m_dense.data();
    const auto end = beg + m_dense.size();
    return component >= beg && component < end ? m_entities[static_cast<size_t>(component - beg)]
                                               : Entity::Null();
}

template<class T>
auto SparseSet<T>::GetIndex(Entity entity) const -> index_type
{
    return m_sparse.at(entity.Index());
}

template<class T>
auto SparseSet<T>::GetEntity(index_type index) const -> Entity
{
    return m_entities.at(m_sparse.at(index));
}

template<class T>
auto SparseSet<T>::Contains(Entity entity) const noexcept -> bool
{
    const auto i = entity.Index();
    return i < m_sparse.size() ? m_sparse[i] != Entity::NullIndex : false;
}

template<class T>
void SparseSet<T>::Swap(Entity lhs, Entity rhs)
{
    if (lhs == rhs)
        return;

    const auto sparseIndex1 = lhs.Index();
    const auto sparseIndex2 = rhs.Index();
    const auto denseIndex1 = m_sparse.at(sparseIndex1);
    const auto denseIndex2 = m_sparse.at(sparseIndex2);
    std::swap(m_sparse.at(sparseIndex1), m_sparse.at(sparseIndex2));
    std::swap(m_entities.at(denseIndex1), m_entities.at(denseIndex2));
}

template<class T>
void SparseSet<T>::Reserve(size_t capacity)
{
    m_dense.reserve(capacity);
    m_entities.reserve(capacity);
}

template<class T>
template<std::predicate<const T&, const T&> Predicate>
void SparseSet<T>::Sort(Predicate&& lessThan)
{
    /** Create array of indices for an out-of-place sort. */
    const auto size = m_dense.size();
    auto permutation = std::vector<uint32_t>(size);
    const auto beg = permutation.begin();
    const auto end = permutation.end();
    std::iota(beg, end, 0u);

    /** Extend the user-provided predicate to operate in terms of indices. */
    auto compare = [&pool = m_dense, predicate = std::forward<Predicate>(lessThan)](const auto lhs,
                                                                                    const auto rhs)
    { return predicate(pool[lhs], pool[rhs]); };

    /** Insertion sort - @todo radix sort used in physics is probably very fast here. */
    for (auto cur = beg; cur != end; ++cur)
    {
        const auto pos = std::upper_bound(beg, cur, *cur, compare);
        std::rotate(pos, cur, cur + 1);
    }

    /** Apply the permutation. */
    for (auto i = 0u; i < size; ++i)
    {
        auto cur = i;
        auto next = permutation[cur];

        while (cur != next)
        {
            const auto i1 = permutation[cur];
            const auto i2 = permutation[next];
            std::swap(m_dense[i1], m_dense[i2]);
            std::swap(m_entities[i1], m_entities[i2]);
            const auto sparse1 = m_entities[i1].Index();
            const auto sparse2 = m_entities[i2].Index();
            m_sparse[sparse1] = i2;
            m_sparse[sparse2] = i1;
            permutation[cur] = cur;
            cur = next;
            next = permutation[cur];
        }
    }
}

template<class T>
void SparseSet<T>::ClearNonPersistent()
{
    std::ranges::fill(m_sparse, Entity::NullIndex);
    auto swapToIndex = 0u;
    for (auto [denseIndex, entity] : algo::Enumerate(m_entities))
    {
        if (!entity.IsPersistent())
            continue;

        // push persistent entities/components to the front
        m_sparse.at(entity.Index()) = swapToIndex;
        m_dense.at(swapToIndex) = std::move(m_dense.at(denseIndex));
        m_entities.at(swapToIndex) = entity;
        ++swapToIndex;
    }

    m_dense.erase(m_dense.begin() + swapToIndex, m_dense.end());
    m_dense.shrink_to_fit();
    m_entities.erase(m_entities.begin() + swapToIndex, m_entities.end());
    m_entities.shrink_to_fit();
}

template<class T>
void SparseSet<T>::Clear()
{
    std::ranges::fill(m_sparse, Entity::NullIndex);
    m_dense.clear();
    m_dense.shrink_to_fit();
    m_entities.clear();
    m_entities.shrink_to_fit();
}
} // namespace nc::ecs::detail
/** @endcond */
