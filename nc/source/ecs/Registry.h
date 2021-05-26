#pragma once

#include "entity/EntityHandle.h"
#include "EntitySystem.h"

#include <algorithm>
#include <functional>
#include <span>
#include <stdexcept>
#include <vector>

/** The regsitry is a collection of entity and component state.
 * 
 *  State tracked per component type in the registry:
 *  - A sparse array of indices into the pools which can be indexed with an entity index.
 * 
 *  - A packed array of all the entity indices associated with the component with the same
 *    ordering as the component pool. Useful for sorting different component pools according
 *    to entity.
 * 
 *  - A packed array of the components.
 * 
 *  - A set of callbacks for external systems to hook into addition and removal events.
 * 
 * 
 *  Note about pointers and iteration:
 *   - Generally don't make assumptions about the ordering of components or cache pointers to them.
 *  
 *   - Adding a component may resize the internal storage, invalidating all pointers and spans. The
 *     ReserveHeadroom method can be used to prevent this if the maximum required capacity is known,
 *     but note that the method itself may cause resizing.
 * 
 *   - Removing a component uses the 'swap and pop' idiom so pointers to the last element and end
 *     iterators are invalidated.
 *  
 *  General Notes
 *   - Newly added and destroyed entities exist in staging areas until CommitStagedChanges is called.
 *     Staged additions are queryable with GetEntity, but they will not appear in the active set. Staged
 *     removals are not queryable, nor do they appear in the active set, but their components are still
 *     present in their respective pools.
 * 
 *   - On destruction of an entity, all components are removed in the order they appear in the
 *     registry's template argument list. The entity is destroyed after all components are removed.
 *     Dependencies on destruction order of components should be avoided.
 * 
 *  @todo
 *   - Sorting
 *   - Buffering
 *   - Groups
 *   - Remove multiple?
 *   - Move entity exists test from Ecs to here?
 *   - more unit tests
 */

namespace nc::ecs
{
    template<class T>
    struct SystemCallbacks
    {
        using on_add_type = std::function<void(const T&)>;
        using on_remove_type = std::function<void(EntityHandle)>;
        on_add_type OnAdd = nullptr;
        on_remove_type OnRemove = nullptr;
    };

    template<class T>
    struct PerComponentStorage
    {
        using index_type = HandleTraits::index_type;
        static constexpr index_type NullIndex = std::numeric_limits<index_type>::max();
        std::vector<index_type> sparseArray;
        std::vector<index_type> entityPool;
        std::vector<T> componentPool;
        SystemCallbacks<T> callbacks;

        PerComponentStorage(size_t maxEntities)
            : sparseArray(maxEntities, NullIndex),
              entityPool{},
              componentPool{},
              callbacks{}
        {}

        void Clear()
        {
            std::ranges::fill(sparseArray, NullIndex);
            entityPool.clear();
            entityPool.shrink_to_fit();
            componentPool.clear();
            componentPool.shrink_to_fit();
        }
    };

    template<class... Ts>
    class Registry
    {
        using storage_type = std::tuple<PerComponentStorage<Ts>...>;
        using index_type = HandleTraits::index_type;
        static constexpr index_type NullIndex = std::numeric_limits<index_type>::max();
        static constexpr size_t PoolCount = sizeof...(Ts);

        public:
            Registry(size_t maxEntities);

            auto CreateEntity(EntityInfo info) -> EntityHandle;
            void DestroyEntity(EntityHandle handle);
            bool EntityExists(EntityHandle handle);
            auto GetEntity(EntityHandle handle) -> Entity*;
            auto GetEntity(const std::string& tag) -> Entity*;
            auto GetActiveEntities() -> std::span<Entity*>;

            template<class T, class... Args>
            auto AddComponent(EntityHandle handle, Args&&... args) -> T*;

            template<class T>
            bool RemoveComponent(EntityHandle handle);

            template<class T>
            bool HasComponent(EntityHandle handle);

            template<class T>
            auto GetComponent(EntityHandle handle) -> T*;

            template<class T>
            auto ViewAll() -> std::span<T>;

            template<class T>
            void ReserveHeadroom(size_t additionalRequiredCount);

            template<class T>
            void RegisterOnAddCallback(SystemCallbacks<T>::on_add_type func);

            template<class T>
            void RegisterOnRemoveCallback(SystemCallbacks<T>::on_remove_type func);

            void CommitStagedChanges();
            void Clear();

        private:
            EntitySystem m_entitySystem;
            storage_type m_storage;

            template<class T>
            auto GetStorageFor() -> PerComponentStorage<T>& { return std::get<PerComponentStorage<T>>(m_storage); }
    };

    template<class... Ts>
    Registry<Ts...>::Registry(size_t maxEntities)
        : m_entitySystem{maxEntities},
          m_storage{PerComponentStorage<Ts>{maxEntities}...}
    {
    }

    template<class... Ts>
    auto Registry<Ts...>::CreateEntity(EntityInfo info) -> EntityHandle
    {
        auto handle = m_entitySystem.Add(info);
        AddComponent<Transform>(handle, info.position, info.rotation, info.scale, info.parent);
        return handle;
    }

    template<class... Ts>
    void Registry<Ts...>::DestroyEntity(EntityHandle handle)
    {
        m_entitySystem.Remove(handle);
    }

    template<class... Ts>
    bool Registry<Ts...>::EntityExists(EntityHandle handle)
    {
        return m_entitySystem.Contains(handle);
    }

    template<class... Ts>
    auto Registry<Ts...>::GetEntity(EntityHandle handle) -> Entity*
    {
        return m_entitySystem.Get(handle);
    }

    template<class... Ts>
    auto Registry<Ts...>::GetEntity(const std::string& tag) -> Entity*
    {
        return m_entitySystem.Get(tag);
    }

    template<class... Ts>
    auto Registry<Ts...>::GetActiveEntities() -> std::span<Entity*>
    {
        return m_entitySystem.GetActiveEntities();
    }

    template<class... Ts>
    template<class T, class... Args>
    T* Registry<Ts...>::AddComponent(EntityHandle handle, Args&&... args)
    {
        if(HasComponent<T>(handle))
            throw std::runtime_error("Registry::AddComponent - Cannot add multiple components of the same type to a single entity");
        
        auto sparseIndex = handle.Index();
        auto& storage = GetStorageFor<T>();
        auto newIndex = storage.componentPool.size();
        auto& newComponent = storage.componentPool.emplace_back(handle, std::forward<Args>(args)...);
        storage.entityPool.push_back(sparseIndex);
        storage.sparseArray.at(sparseIndex) = newIndex;

        if(auto& func = storage.callbacks.OnAdd; func)
            func(newComponent);
        
        return &newComponent;
    }

    template<class... Ts>
    template<class T>
    bool Registry<Ts...>::RemoveComponent(EntityHandle handle)
    {
        auto sparseIndex = handle.Index();
        auto& storage = GetStorageFor<T>();
        auto poolIndex = storage.sparseArray.at(sparseIndex);
        if(poolIndex == NullIndex)
            return false;
        
        auto& componentPool = storage.componentPool;
        componentPool.at(poolIndex) = std::move(componentPool.back());
        componentPool.pop_back();

        auto& entityPool = storage.entityPool;
        auto movedIndex = entityPool.back(); // need to store in case we're removing the last element
        entityPool.at(poolIndex) = movedIndex;
        entityPool.pop_back();

        storage.sparseArray.at(sparseIndex) = NullIndex;

        if(sparseIndex != movedIndex)
            storage.sparseArray.at(movedIndex) = poolIndex;

        if(auto& func = storage.callbacks.OnRemove; func)
            func(handle);

        return true;
    }

    template<class... Ts>
    template<class T>
    bool Registry<Ts...>::HasComponent(EntityHandle handle)
    {
        return GetStorageFor<T>().sparseArray.at(handle.Index()) == NullIndex ? false : true;
    }

    template<class... Ts>
    template<class T>
    T* Registry<Ts...>::GetComponent(EntityHandle handle)
    {
        auto& storage = GetStorageFor<T>();
        auto poolIndex = storage.sparseArray.at(handle.Index());
        return poolIndex == NullIndex ? nullptr : &storage.componentPool.at(poolIndex);
    }

    template<class... Ts>
    template<class T>
    std::span<T> Registry<Ts...>::ViewAll()
    {
        return std::span<T>{GetStorageFor<T>().componentPool};
    }

    template<class... Ts>
    template<class T>
    void Registry<Ts...>::ReserveHeadroom(size_t additionalRequiredCount)
    {
        auto& storage = GetStorageFor<T>();
        auto requiredSize = storage.componentPool.size() + additionalRequiredCount;
        storage.componentPool.reserve(requiredSize);
        storage.entityPool.reserve(requiredSize);
    }

    template<class... Ts>
    template<class T>
    void Registry<Ts...>::RegisterOnAddCallback(typename SystemCallbacks<T>::on_add_type func)
    {
        GetStorageFor<T>().callbacks.OnAdd = std::move(func);
    }

    template<class... Ts>
    template<class T>
    void Registry<Ts...>::RegisterOnRemoveCallback(typename SystemCallbacks<T>::on_remove_type func)
    {
        GetStorageFor<T>().callbacks.OnRemove = std::move(func);
    }

    template<class... Ts>
    void Registry<Ts...>::CommitStagedChanges()
    {
        m_entitySystem.CommitRemovals([this](const auto& entity)
        {
            auto handle = entity.Handle;
            (RemoveComponent<Ts>(handle), ...);
        });
    }

    template<class... Ts>
    void Registry<Ts...>::Clear()
    {
        m_entitySystem.Clear();
        std::apply([](auto&&... data) { (data.Clear(), ...); }, m_storage);
    }
}