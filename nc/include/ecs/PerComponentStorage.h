#pragma once

#include "ecs/component/Component.h"
#include "ecs/component/AutoComponentGroup.h"
#include "debug/Utils.h"

#include <algorithm>
#include <functional>
#include <span>
#include <vector>

namespace nc
{
    class PerComponentStorageBase
    {
        public:
            virtual ~PerComponentStorageBase() = default;
            virtual void Clear() = 0;
            virtual void CommitStagedComponents(const std::vector<Entity>& removed) = 0;
            virtual void VerifyCallbacks() = 0;
    };

    template<Component T>
    struct SystemCallbacks
    {
        using on_add_type = std::function<void(T&)>;
        using on_remove_type = std::function<void(Entity)>;
        on_add_type OnAdd = nullptr;
        on_remove_type OnRemove = nullptr;
    };

    template<Component T>
    class PerComponentStorage : public PerComponentStorageBase
    {
        using value_type = T;
        using index_type = Entity::index_type;

        struct StagedComponent
        {
            Entity entity;
            T component;
        };

        public:
            PerComponentStorage(size_t maxEntities);
            ~PerComponentStorage() = default;
            PerComponentStorage(PerComponentStorage&&) = default;
            PerComponentStorage& operator=(PerComponentStorage&&) = default;

            PerComponentStorage(const PerComponentStorage&) = delete;
            PerComponentStorage& operator=(const PerComponentStorage&) = delete;

            auto GetSparseArray() -> std::vector<index_type>& { return sparseArray; }
            auto GetEntityPool() -> std::vector<Entity>& { return entityPool; }
            auto GetComponentPool() -> std::vector<T>& { return componentPool; }

            template<class... Args>
            auto Add(Entity entity, Args&&... args) -> T*;

            void Remove(Entity entity);
            bool TryRemove(Entity entity);
            bool Contains(Entity entity) const;
            auto Get(Entity entity) -> T*;
            auto Get(Entity entity) const -> const T*;
            auto ViewAll() -> std::span<T>;
            auto ViewAll() const -> std::span<const T>;

            void RegisterOnAddCallback(SystemCallbacks<T>::on_add_type func);
            void RegisterOnRemoveCallback(SystemCallbacks<T>::on_remove_type func);
            void Swap(index_type firstEntity, index_type secondEntity);
            void ReserveHeadroom(size_t additionalRequiredCount);
            
            void Clear() override;
            void CommitStagedComponents(const std::vector<Entity>& removed) override;
            void VerifyCallbacks() override;

        private:
            std::vector<index_type> sparseArray;
            std::vector<Entity> entityPool;
            std::vector<T> componentPool;
            std::vector<StagedComponent> stagingPool;
            SystemCallbacks<T> callbacks;
    };

    template<Component T>
    PerComponentStorage<T>::PerComponentStorage(size_t maxEntities)
        : sparseArray(maxEntities, Entity::NullIndex),
          entityPool{},
          componentPool{},
          stagingPool{},
          callbacks{}
    {}

    template<Component T>
    template<class... Args>
    auto PerComponentStorage<T>::Add(Entity entity, Args&&... args) -> T*
    {
        IF_THROW(Contains(entity), "PerComponentStorage::Add - Cannot add multiple components of the same type");
        auto& [emplacedEntity, emplacedComponent] = stagingPool.emplace_back(entity, T{entity, std::forward<Args>(args)...});

        if constexpr(StoragePolicy<T>::requires_on_add_callback::value)
            callbacks.OnAdd(emplacedComponent);

        return &emplacedComponent;
    }

    template<Component T>
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

        if constexpr(StoragePolicy<T>::requires_on_remove_callback::value)
            callbacks.OnRemove(entity);
    }

    template<Component T>
    bool PerComponentStorage<T>::TryRemove(Entity entity)
    {
        if(!Contains(entity))
            return false;
        
        Remove(entity);
        return true;
    }

    template<Component T>
    bool PerComponentStorage<T>::Contains(Entity entity) const
    {
        if(sparseArray.at(entity.Index()) != Entity::NullIndex)
            return true;

        auto pos = std::ranges::find_if(stagingPool, [entity](const auto& pair)
        {
            return entity == pair.entity;
        });

        return pos == stagingPool.cend() ? false : true;
    }

    template<Component T>
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

    template<Component T>
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

    template<Component T>
    auto PerComponentStorage<T>::ViewAll() -> std::span<T>
    {
        return std::span<T>{componentPool};
    }
    
    template<Component T>
    auto PerComponentStorage<T>::ViewAll() const -> std::span<const T>
    {
        return std::span<const T>{componentPool};
    }

    template<Component T>
    void PerComponentStorage<T>::RegisterOnAddCallback(typename SystemCallbacks<T>::on_add_type func)
    {
        callbacks.OnAdd = std::move(func);
    }

    template<Component T>
    void PerComponentStorage<T>::RegisterOnRemoveCallback(typename SystemCallbacks<T>::on_remove_type func)
    {
        callbacks.OnRemove = std::move(func);
    }

    template<Component T>
    void PerComponentStorage<T>::VerifyCallbacks()
    {
        if constexpr(StoragePolicy<T>::requires_on_add_callback::value)
        {
            if(!callbacks.OnAdd)
                throw NcError("OnAdd callback required but not set");
        }

        if constexpr(StoragePolicy<T>::requires_on_remove_callback::value)
        {
            if(!callbacks.OnRemove)
                throw NcError("OnRemove callback required but not set");
        }
    }

    template<Component T>
    void PerComponentStorage<T>::Swap(index_type firstEntity, index_type secondEntity)
    {
        auto firstDenseIndex = sparseArray.at(firstEntity);
        auto secondDenseIndex = sparseArray.at(secondEntity);
        std::swap(entityPool.at(firstDenseIndex), entityPool.at(secondDenseIndex));
        std::swap(componentPool.at(firstDenseIndex), componentPool.at(secondDenseIndex));
        sparseArray.at(firstEntity) = secondDenseIndex;
        sparseArray.at(secondEntity) = firstDenseIndex;
    }

    template<Component T>
    void PerComponentStorage<T>::ReserveHeadroom(size_t additionalRequiredCount)
    {
        auto requiredSize = componentPool.size() + additionalRequiredCount;
        componentPool.reserve(requiredSize);
        entityPool.reserve(requiredSize);
    }

    template<Component T>
    void PerComponentStorage<T>::CommitStagedComponents(const std::vector<Entity>& removed)
    {
        for(auto entity : removed)
            TryRemove(entity);

        for(auto& [entity, component] : stagingPool)
        {
            componentPool.push_back(std::move(component));
            auto sparseIndex = entity.Index();
            entityPool.push_back(entity);
            sparseArray.at(sparseIndex) = componentPool.size() - 1;
        }

        stagingPool.clear();

        if constexpr(std::same_as<T, AutoComponentGroup>)
        {
            for(auto& group : componentPool)
            {
                group.CommitStagedComponents();
            }
        }
    }

    template<Component T>
    void PerComponentStorage<T>::Clear()
    {
        assert(stagingPool.empty());
        stagingPool.shrink_to_fit();

        std::ranges::fill(sparseArray, Entity::NullIndex);

        size_t swapToIndex = 0u;
        for(size_t denseIndex = 0u; denseIndex < entityPool.size(); ++denseIndex)
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