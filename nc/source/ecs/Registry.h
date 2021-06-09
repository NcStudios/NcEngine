#pragma once

#include "entity/HandleUtils.h"
#include "EntitySystem.h"

#include <algorithm>
#include <functional>
#include <span>
#include <stdexcept>
#include <vector>

/** The regsitry is a collection of entity and component state.
 * 
 *  State tracked per component type in the registry:
 *  - A packed array of the components.
 * 
 *  - A packed array of all the entity indices associated with the component with the same
 *    ordering as the component pool. Useful for sorting different component pools according
 *    to entity.
 * 
 *  - A sparse array of indices into the pools which can be indexed with an entity index.
 * 
 *  - A set of callbacks for external systems to hook into addition and removal events.
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
 *   - Callbacks will only be used if specified in the StoragePolicy for a component. Attempts to 
 *     set callbacks for types whose StoragePolicy doesn't allow this will fail to compile, while 
 *     VerifyCallbacks can be used to check at runtime that all required callbacks are set, throwing
 *     on failure.
 *
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
 *   - Finish re-fetching pointer?
 *   - Sorting
 *   - Buffering
 *   - Groups
 *   - Remove multiple?
 *   - Move entity exists IF_THROWs from Ecs to here?
 *   - more unit tests
 *   - tuple_cat to combine engine/user component lists
 */

namespace nc::ecs
{
    template<class T>
    struct SystemCallbacks
    {
        using on_add_type = std::function<void(T&)>;
        using on_remove_type = std::function<void(EntityHandle)>;
        on_add_type OnAdd = nullptr;
        on_remove_type OnRemove = nullptr;
    };

    template<class T>
    struct PerComponentStorage
    {
        using index_type = HandleTraits::index_type;
        std::vector<index_type> sparseArray;
        std::vector<index_type> entityPool;
        std::vector<T> componentPool;
        SystemCallbacks<T> callbacks;

        PerComponentStorage(size_t maxEntities)
            : sparseArray(maxEntities, HandleTraits::NullIndex),
              entityPool{},
              componentPool{},
              callbacks{}
        {}

        ~PerComponentStorage() = default;
        PerComponentStorage(PerComponentStorage&&) = default;
        PerComponentStorage& operator=(PerComponentStorage&&) = default;

        PerComponentStorage(const PerComponentStorage&) = delete;
        PerComponentStorage& operator=(const PerComponentStorage&) = delete;

        void Clear()
        {
            std::ranges::fill(sparseArray, HandleTraits::NullIndex);
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
        static constexpr size_t PoolCount = sizeof...(Ts);

        public:
            Registry(size_t maxEntities);
            ~Registry() = default;
            Registry(Registry&&) = default;
            Registry& operator=(Registry&&) = default; 

            Registry(const Registry&) = delete;
            Registry& operator=(const Registry&) = delete;

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
            void VerifyCallbacks();
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
        
        auto& storage = GetStorageFor<T>();
        auto& newComponent = storage.componentPool.emplace_back(handle, std::forward<Args>(args)...);
        auto sparseIndex = HandleUtils::Index(handle);
        storage.entityPool.push_back(sparseIndex);
        storage.sparseArray.at(sparseIndex) = storage.componentPool.size() - 1;

        if constexpr(StoragePolicy<T>::requires_on_add_callback::value)
        {
            storage.callbacks.OnAdd(newComponent);
        }

        return &newComponent;
    }

    template<class... Ts>
    template<class T>
    bool Registry<Ts...>::RemoveComponent(EntityHandle handle)
    {
        auto sparseIndex = HandleUtils::Index(handle);
        auto& storage = GetStorageFor<T>();
        auto poolIndex = storage.sparseArray.at(sparseIndex);
        if(poolIndex == HandleTraits::NullIndex)
            return false;
        
        auto& componentPool = storage.componentPool;
        componentPool.at(poolIndex) = std::move(componentPool.back());
        componentPool.pop_back();

        auto& entityPool = storage.entityPool;
        auto movedIndex = entityPool.back(); // need to store in case we're removing the last element
        entityPool.at(poolIndex) = movedIndex;
        entityPool.pop_back();

        storage.sparseArray.at(sparseIndex) = HandleTraits::NullIndex;

        if(sparseIndex != movedIndex)
            storage.sparseArray.at(movedIndex) = poolIndex;

        if constexpr(StoragePolicy<T>::requires_on_remove_callback::value)
        {
            storage.callbacks.OnRemove(handle);
        }

        return true;
    }

    template<class... Ts>
    template<class T>
    bool Registry<Ts...>::HasComponent(EntityHandle handle)
    {
        return GetStorageFor<T>().sparseArray.at(HandleUtils::Index(handle)) == HandleTraits::NullIndex ? false : true;
    }

    template<class... Ts>
    template<class T>
    T* Registry<Ts...>::GetComponent(EntityHandle handle)
    {
        auto& storage = GetStorageFor<T>();
        auto poolIndex = storage.sparseArray.at(HandleUtils::Index(handle));
        return poolIndex == HandleTraits::NullIndex ? nullptr : &storage.componentPool.at(poolIndex);
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
        static_assert(StoragePolicy<T>::requires_on_add_callback::value, "Cannot register an OnAdd callback unless specified in the StoragePolicy");
        GetStorageFor<T>().callbacks.OnAdd = std::move(func);
    }

    template<class... Ts>
    template<class T>
    void Registry<Ts...>::RegisterOnRemoveCallback(typename SystemCallbacks<T>::on_remove_type func)
    {
        static_assert(StoragePolicy<T>::requires_on_remove_callback::value, "Cannot register an OnRemove callback unless specified in the StoragePolicy");
        GetStorageFor<T>().callbacks.OnRemove = std::move(func);
    }

    template<class... Ts>
    void Registry<Ts...>::CommitStagedChanges()
    {
        m_entitySystem.CommitRemovals([this](const auto& entity)
        {
            (RemoveComponent<Ts>(entity.Handle), ...);
        });
    }

    template<class... Ts>
    void Registry<Ts...>::Clear()
    {
        m_entitySystem.Clear();
        std::apply([](auto&&... data) { (data.Clear(), ...); }, m_storage);
    }

    template<class... Ts>
    void Registry<Ts...>::VerifyCallbacks()
    {
        auto CallbackCheck = [] <class T> (const PerComponentStorage<T>& data)
        {
            if constexpr(StoragePolicy<T>::requires_on_add_callback::value)
            {
                if(!data.callbacks.OnAdd)
                    throw std::runtime_error("OnAdd callback required but not set\n   " + std::string{__PRETTY_FUNCTION__});
            }

            if constexpr(StoragePolicy<T>::requires_on_remove_callback::value)
            {
                if(!data.callbacks.OnRemove)
                    throw std::runtime_error("OnRemove callback required but not set\n   " + std::string{__PRETTY_FUNCTION__});
            }
        };

        std::apply([&CallbackCheck](auto&&... data) { (CallbackCheck(data), ...); }, m_storage);
    }
}