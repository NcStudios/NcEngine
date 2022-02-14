#pragma once

#include "ecs/component/Component.h"
#include "ecs/component/FreeComponentGroup.h"
#include "debug/Utils.h"

#include <algorithm>
#include <functional>
#include <numeric>
#include <span>
#include <vector>

namespace nc::detail
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
            auto size() const noexcept { return entityPool.size(); }

            virtual ~PerComponentStorageBase() = default;
            virtual void Clear() = 0;
            virtual void CommitStagedComponents(const std::vector<Entity>& removed) = 0;
            virtual void VerifyCallbacks() = 0;

        protected:
            std::vector<index_type> sparseArray;
            std::vector<Entity> entityPool;
    };

    template<PooledComponent T>
    struct SystemCallbacks
    {
        using on_add_type = std::function<void(T&)>;
        using on_remove_type = std::function<void(Entity)>;
        on_add_type OnAdd = nullptr;
        on_remove_type OnRemove = nullptr;
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

            auto ComponentPool() noexcept -> std::vector<T>& { return componentPool; }
            auto ComponentPool() const noexcept -> const std::vector<T>& { return componentPool; }

            template<class... Args>
            auto Add(Entity entity, Args&&... args) -> T*;

            void Remove(Entity entity);
            bool TryRemove(Entity entity);
            bool Contains(Entity entity) const;
            auto Get(Entity entity) -> T*;
            auto Get(Entity entity) const -> const T*;
            //auto ViewAll() -> std::span<T>;
            //auto ViewAll() const -> std::span<const T>;

            template<std::predicate<const T&, const T&> Predicate>
            void Sort(Predicate&& comparesLessThan);

            void RegisterOnAddCallback(SystemCallbacks<T>::on_add_type func);
            void RegisterOnRemoveCallback(SystemCallbacks<T>::on_remove_type func);
            void Swap(index_type firstEntity, index_type secondEntity);
            void ReserveHeadroom(size_t additionalRequiredCount);

            void Clear() override;
            void CommitStagedComponents(const std::vector<Entity>& removed) override;
            void VerifyCallbacks() override;

        private:
            std::vector<T> componentPool;
            std::vector<StagedComponent> stagingPool;
            SystemCallbacks<T> callbacks;
    };

    template<PooledComponent T>
    PerComponentStorage<T>::PerComponentStorage(size_t maxEntities)
        : PerComponentStorageBase{maxEntities},
          componentPool{},
          stagingPool{},
          callbacks{}
    {}

    template<PooledComponent T>
    template<class... Args>
    auto PerComponentStorage<T>::Add(Entity entity, Args&&... args) -> T*
    {
        IF_THROW(Contains(entity), "PerComponentStorage::Add - Cannot add multiple components of the same type");
        auto& [emplacedEntity, emplacedComponent] = stagingPool.emplace_back(entity, T{entity, std::forward<Args>(args)...});

        if constexpr(storage_policy<T>::requires_on_add_callback)
            callbacks.OnAdd(emplacedComponent);

        return &emplacedComponent;
    }

    template<PooledComponent T>
    void PerComponentStorage<T>::Remove(Entity entity)
    {
        auto sparseIndex = entity.Index();
        auto poolIndex = sparseArray.at(sparseIndex);

        IF_THROW(poolIndex == Entity::NullIndex, "Entity does not have component");

        componentPool.at(poolIndex) = std::move(componentPool.back());
        componentPool.pop_back();

        auto movedEntity = entityPool.back(); // need to store in case we're removing the last element
        entityPool.at(poolIndex) = movedEntity;
        entityPool.pop_back();

        sparseArray.at(sparseIndex) = Entity::NullIndex;

        if(sparseIndex != movedEntity.Index())
            sparseArray.at(movedEntity.Index()) = poolIndex;

        if constexpr(storage_policy<T>::requires_on_remove_callback)
            callbacks.OnRemove(entity);
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

        return stagingPool.cend() != std::ranges::find_if(stagingPool, [entity](const auto& pair)
        {
            return entity == pair.entity;
        });
    }

    template<PooledComponent T>
    auto PerComponentStorage<T>::Get(Entity entity) -> T*
    {
        auto poolIndex = sparseArray.at(entity.Index());
        if(poolIndex != Entity::NullIndex)
            return &componentPool.at(poolIndex);

        auto pos = std::ranges::find_if(stagingPool, [entity](const auto& pair)
        {
            return pair.entity == entity;
        });

        return pos == stagingPool.end() ? nullptr : &pos->component;
    }

    template<PooledComponent T>
    auto PerComponentStorage<T>::Get(Entity entity) const -> const T*
    {
        auto poolIndex = sparseArray.at(entity.Index());
        if(poolIndex != Entity::NullIndex)
            return &componentPool.at(poolIndex);

        const auto pos = std::ranges::find_if(stagingPool, [entity](const auto& pair)
        {
            return pair.entity == entity;
        });

        return pos == stagingPool.end() ? nullptr : &pos->component;
    }

    // template<PooledComponent T>
    // auto PerComponentStorage<T>::ViewAll() -> std::span<T>
    // {
    //     return std::span<T>{componentPool};
    // }

    // template<PooledComponent T>
    // auto PerComponentStorage<T>::ViewAll() const -> std::span<const T>
    // {
    //     return std::span<const T>{componentPool};
    // }

    template<PooledComponent T>
    template<std::predicate<const T&, const T&> Predicate>
    void PerComponentStorage<T>::Sort(Predicate&& comparesLessThan)
    {
        /** Create array of indices for an out-of-place sort. */
        const auto size = componentPool.size();
        std::vector<uint32_t> permutation(size);
        const auto beg = permutation.begin();
        const auto end = permutation.end();
        std::iota(beg, end, 0u);

        /** Extend the user-provided predicate to operate in terms of indices. */
        auto compare = [&pool = componentPool, predicate = std::forward<Predicate>(comparesLessThan)](const auto lhs, const auto rhs)
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
                std::swap(componentPool[i1], componentPool[i2]);
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
    void PerComponentStorage<T>::RegisterOnAddCallback(typename SystemCallbacks<T>::on_add_type func)
    {
        callbacks.OnAdd = std::move(func);
    }

    template<PooledComponent T>
    void PerComponentStorage<T>::RegisterOnRemoveCallback(typename SystemCallbacks<T>::on_remove_type func)
    {
        callbacks.OnRemove = std::move(func);
    }

    template<PooledComponent T>
    void PerComponentStorage<T>::VerifyCallbacks()
    {
        if constexpr(storage_policy<T>::requires_on_add_callback)
        {
            if(!callbacks.OnAdd)
                throw NcError("OnAdd callback required but not set");
        }

        if constexpr(storage_policy<T>::requires_on_remove_callback)
        {
            if(!callbacks.OnRemove)
                throw NcError("OnRemove callback required but not set");
        }
    }

    template<PooledComponent T>
    void PerComponentStorage<T>::Swap(index_type firstEntity, index_type secondEntity)
    {
        auto firstDenseIndex = sparseArray.at(firstEntity);
        auto secondDenseIndex = sparseArray.at(secondEntity);
        std::swap(entityPool.at(firstDenseIndex), entityPool.at(secondDenseIndex));
        std::swap(componentPool.at(firstDenseIndex), componentPool.at(secondDenseIndex));
        sparseArray.at(firstEntity) = secondDenseIndex;
        sparseArray.at(secondEntity) = firstDenseIndex;
    }

    template<PooledComponent T>
    void PerComponentStorage<T>::ReserveHeadroom(size_t additionalRequiredCount)
    {
        auto requiredSize = componentPool.size() + additionalRequiredCount;
        componentPool.reserve(requiredSize);
        entityPool.reserve(requiredSize);
    }

    template<PooledComponent T>
    void PerComponentStorage<T>::CommitStagedComponents(const std::vector<Entity>& removed)
    {
        for(auto entity : removed)
            TryRemove(entity);

        for(auto& [entity, component] : stagingPool)
        {
            componentPool.push_back(std::move(component));
            auto sparseIndex = entity.Index();
            entityPool.push_back(entity);
            sparseArray.at(sparseIndex) = static_cast<uint32_t>(componentPool.size() - 1);
        }

        stagingPool.clear();

        if constexpr(std::same_as<T, FreeComponentGroup>)
        {
            for(auto& group : componentPool)
            {
                group.CommitStagedComponents();
            }
        }
    }

    template<PooledComponent T>
    void PerComponentStorage<T>::Clear()
    {
        assert(stagingPool.empty());
        stagingPool.shrink_to_fit();

        std::ranges::fill(sparseArray, Entity::NullIndex);

        uint32_t swapToIndex = 0u;
        for(uint32_t denseIndex = 0u; denseIndex < entityPool.size(); ++denseIndex)
        {
            auto entity = entityPool[denseIndex];
            if(!entity.IsPersistent())
                continue;

            sparseArray.at(entity.Index()) = swapToIndex;
            entityPool.at(swapToIndex) = entityPool.at(denseIndex);
            componentPool.at(swapToIndex) = std::move(componentPool.at(denseIndex));
            ++swapToIndex;
        }

        entityPool.erase(entityPool.begin() + swapToIndex, entityPool.end());
        entityPool.shrink_to_fit();
        componentPool.erase(componentPool.begin() + swapToIndex, componentPool.end());
        componentPool.shrink_to_fit();
    }
}