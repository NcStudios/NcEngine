#pragma once

#include "Entity.h"
#include "HandleManager.h"
#include "AutoComponentGroup.h"
#include "Tag.h"
#include "Transform.h"

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
 *  - A staging array of recently added components waiting to be merged into the pool.
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
 *     present in their respective pools until destruction is finalized.
 *
 *   - Newly added components also exist in a staging area to allow adding components while iterating
 *     over pools. Calls to Contains and Get work normally while a component is staged, but Remove
 *     will only work when called in a later frame.
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
        using on_remove_type = std::function<void(Entity)>;
        on_add_type OnAdd = nullptr;
        on_remove_type OnRemove = nullptr;
    };

    template<class T>
    struct PerComponentStorage
    {
        using index_type = EntityTraits::index_type;
        
        struct StagedComponent
        {
            Entity entity;
            T component;
        };

        std::vector<index_type> sparseArray;
        std::vector<index_type> entityPool;
        std::vector<T> componentPool;
        std::vector<StagedComponent> stagingPool;
        SystemCallbacks<T> callbacks;

        PerComponentStorage(size_t maxEntities)
            : sparseArray(maxEntities, EntityTraits::NullIndex),
              entityPool{},
              componentPool{},
              stagingPool{},
              callbacks{}
        {}

        ~PerComponentStorage() = default;
        PerComponentStorage(PerComponentStorage&&) = default;
        PerComponentStorage& operator=(PerComponentStorage&&) = default;

        PerComponentStorage(const PerComponentStorage&) = delete;
        PerComponentStorage& operator=(const PerComponentStorage&) = delete;

        void Clear()
        {
            std::ranges::fill(sparseArray, EntityTraits::NullIndex);
            entityPool.clear();
            entityPool.shrink_to_fit();
            componentPool.clear();
            componentPool.shrink_to_fit();
            stagingPool.clear();
            stagingPool.shrink_to_fit();
        }
    };

    template<class... Ts>
    class Registry
    {
        using storage_type = std::tuple<PerComponentStorage<Ts>...>;
        using index_type = EntityTraits::index_type;
        static constexpr size_t PoolCount = sizeof...(Ts);

        public:
            Registry(size_t maxEntities);
            ~Registry() = default;
            Registry(Registry&&) = default;
            Registry& operator=(Registry&&) = default; 

            Registry(const Registry&) = delete;
            Registry& operator=(const Registry&) = delete;

            /** Entity Functions */
            template<std::same_as<Entity> T>
            auto Add(EntityInfo info) -> Entity;

            template<std::same_as<Entity> T>
            bool Remove(Entity entity);

            template<std::same_as<Entity> T>
            bool Contains(Entity entity);

            template<std::same_as<Entity> T>
            auto ViewAll() -> std::span<Entity>;

            /** Component Functions */
            template<class T, class... Args>
            auto Add(Entity entity, Args&&... args) -> T*;

            template<class T>
            bool Remove(Entity entity);

            template<class T>
            bool Contains(Entity entity) const;

            template<class T>
            auto Get(Entity entity) -> T*;

            template<class T>
            auto Get(Entity entity) const -> const T*;

            template<class T>
            auto ViewAll() -> std::span<T>;

            template<class T>
            auto ViewAll() const -> std::span<const T>;

            template<class T, class U>
            void Sort();

            template<class T>
            void ReserveHeadroom(size_t additionalRequiredCount);

            /** AutoComponent Functions */
            template<std::derived_from<AutoComponent> T, class ... Args>
            auto Add(Entity entity, Args&& ... args) -> T*;

            template<std::derived_from<AutoComponent> T>
            bool Remove(Entity entity);

            template<std::derived_from<AutoComponent> T>
            bool Contains(Entity entity) const;

            template<std::derived_from<AutoComponent> T>
            auto Get(Entity entity) -> T*;

            template<std::derived_from<AutoComponent> T>
            auto Get(Entity entity) const -> const T*;

            /** System Functions */
            template<class T>
            void RegisterOnAddCallback(SystemCallbacks<T>::on_add_type func);

            template<class T>
            void RegisterOnRemoveCallback(SystemCallbacks<T>::on_remove_type func);

            /** Engine Functions */
            void CommitStagedChanges();
            void VerifyCallbacks();
            void Clear();

        private:
            storage_type m_storage;
            std::vector<Entity> m_active;
            std::vector<Entity> m_toAdd;
            std::vector<Entity> m_toRemove;
            HandleManager m_handleManager;

            template<class T>
            void CommitStagedComponents();

            template<class T>
            auto GetStorageFor() -> PerComponentStorage<T>& { return std::get<PerComponentStorage<T>>(m_storage); }

            template<class T>
            auto GetStorageFor() const -> const PerComponentStorage<T>& { return std::get<PerComponentStorage<T>>(m_storage); }
    };

    template<class... Ts>
    Registry<Ts...>::Registry(size_t maxEntities)
        : m_storage{PerComponentStorage<Ts>{maxEntities}...},
          m_active{},
          m_toAdd{},
          m_toRemove{},
          m_handleManager{}
    {
    }

    template<class... Ts>
    template<class Reference, class Target>
    void Registry<Ts...>::Sort()
    {
        // force committing changes?

        auto& referenceStorage = GetStorageFor<Reference>();
        auto& targetStorage = GetStorageFor<Target>();

        auto& sortedSparseArray = referenceStorage.sparseArray;
        auto& sortedEntityPool = referenceStorage.entityPool;
        auto& sortedComponents = referenceStorage.componentPool;
        auto& unsortedSparseArray = targetStorage.sparseArray;
        auto& unsortedEntityPool = targetStorage.entityPool;
        auto& unsortedComponents = targetStorage.componentPool;

        size_t sortedSize = sortedEntityPool.size();
        if(sortedSize == 0u || unsortedEntityPool.size() == 0u)
            return;

        size_t currentIndex = 0u;
        size_t endIndex = sortedSize - 1;

        while(currentIndex < endIndex)
        {
            auto entityIndex = sortedEntityPool.at(currentIndex);

            auto unsortedIndex = unsortedSparseArray.at(entityIndex);

            // both have entity
            if(unsortedIndex != EntityTraits::NullIndex)
            {
                // check if sorted?


                auto sortedIndex = sortedSparseArray.at(entityIndex);
                
                // swap entity
                auto tempEntity = unsortedEntityPool.at(sortedIndex);
                unsortedEntityPool.at(sortedIndex) = unsortedEntityPool.at(unsortedIndex);
                unsortedEntityPool.at(unsortedIndex) = tempEntity;

                // swap component
                auto tempComponent = std::move(unsortedComponents.at(sortedIndex));
                unsortedComponents.at(sortedIndex) = std::move(unsortedComponents.at(unsortedIndex));
                unsortedComponents.at(unsortedIndex) = std::move(tempComponent);

                // update sparse
                unsortedSparseArray.at(entityIndex) = sortedIndex;
                unsortedSparseArray.at(tempEntity) = unsortedIndex;

                ++currentIndex;
            }
            // entity only in sorted
            else
            {
                // swap entity to end
                sortedEntityPool.at(currentIndex) = sortedEntityPool.at(endIndex);
                sortedEntityPool.at(endIndex) = entityIndex;

                // swap component to end
                auto temp = std::move(sortedComponents.at(currentIndex));
                sortedComponents.at(currentIndex) = std::move(sortedComponents.at(endIndex));
                sortedComponents.at(endIndex) = std::move(temp);

                // update sparse
                sortedSparseArray.at(entityIndex) = endIndex;
                auto movedEntityIndex = sortedEntityPool.at(currentIndex);
                sortedSparseArray.at(movedEntityIndex) = currentIndex;

                --endIndex;
            }
        }
    }

    template<class... Ts>
    template<std::same_as<Entity> T>
    auto Registry<Ts...>::Add(EntityInfo info) -> Entity
    {
        auto handle = m_handleManager.GenerateNewHandle(info.layer, info.flags);
        Add<Transform>(handle, info.position, info.rotation, info.scale, info.parent);
        Add<AutoComponentGroup>(handle);
        Add<Tag>(handle, std::move(info.tag));
        m_toAdd.push_back(handle);
        return handle;
    }

    template<class... Ts>
    template<class T, class... Args>
    auto Registry<Ts...>::Add(Entity entity, Args&&... args) -> T*
    {
        if(Contains<T>(entity))
            throw std::runtime_error("Registry::Add - Cannot add multiple components of the same type to a single entity");
        
        auto& storage = GetStorageFor<T>();
        auto& [emplacedEntity, emplacedComponent] = storage.stagingPool.emplace_back(entity, T{entity, std::forward<Args>(args)...});

        if constexpr(StoragePolicy<T>::requires_on_add_callback::value)
        {
            storage.callbacks.OnAdd(emplacedComponent);
        }

        return &emplacedComponent;
    }

    template<class... Ts>
    template<std::derived_from<AutoComponent> T, class... Args>
    auto Registry<Ts...>::Add(Entity entity, Args&&... args) -> T*
    {
        AutoComponentGroup* group = Get<AutoComponentGroup>(entity);
        if(!group)
            throw std::runtime_error("Registry::Add (AutoComponent) - Group does not exist");

        return group->Add<T>(entity, std::forward<Args>(args)...);

    }

    template<class... Ts>
    template<std::same_as<Entity> T>
    bool Registry<Ts...>::Remove(Entity entity)
    {
        auto pos = std::ranges::find(m_active, entity);

        if(pos == m_active.end())
            return false;
        
        *pos = m_active.back();
        m_active.pop_back();
        m_toRemove.push_back(entity);
        return true;
    }

    template<class... Ts>
    template<class T>
    bool Registry<Ts...>::Remove(Entity entity)
    {
        auto sparseIndex = EntityUtils::Index(entity);
        auto& storage = GetStorageFor<T>();
        auto poolIndex = storage.sparseArray.at(sparseIndex);
        if(poolIndex == EntityTraits::NullIndex)
            return false;
        
        auto& componentPool = storage.componentPool;
        componentPool.at(poolIndex) = std::move(componentPool.back());
        componentPool.pop_back();

        auto& entityPool = storage.entityPool;
        auto movedIndex = entityPool.back(); // need to store in case we're removing the last element
        entityPool.at(poolIndex) = movedIndex;
        entityPool.pop_back();

        storage.sparseArray.at(sparseIndex) = EntityTraits::NullIndex;

        if(sparseIndex != movedIndex)
            storage.sparseArray.at(movedIndex) = poolIndex;

        if constexpr(StoragePolicy<T>::requires_on_remove_callback::value)
        {
            storage.callbacks.OnRemove(entity);
        }

        return true;
    }

    template<class... Ts>
    template<std::derived_from<AutoComponent> T>
    bool Registry<Ts...>::Remove(Entity entity)
    {
        auto& storage = GetStorageFor<AutoComponentGroup>();
        auto poolIndex = storage.sparseArray.at(EntityUtils::Index(entity));
        if(poolIndex == EntityTraits::NullIndex)
            return false;
        
        AutoComponentGroup& group = storage.componentPool.at(poolIndex);
        return group.Remove<T>();
    }

    template<class... Ts>
    template<std::same_as<Entity> T>
    bool Registry<Ts...>::Contains(Entity entity)
    {
        return (m_active.cend() != std::ranges::find(m_active, entity)) ||
               (m_toAdd.cend() != std::ranges::find(m_toAdd, entity));
    }

    template<class... Ts>
    template<class T>
    bool Registry<Ts...>::Contains(Entity entity) const
    {
        auto& storage = GetStorageFor<T>();

        if(EntityTraits::NullIndex != storage.sparseArray.at(EntityUtils::Index(entity)))
            return true;

        auto pos = std::ranges::find_if(storage.stagingPool, [entity](const auto& pair)
        {
            return entity == pair.entity;
        });

        return pos == storage.stagingPool.cend() ? false : true;
    }

    template<class... Ts>
    template<std::derived_from<AutoComponent> T>
    bool Registry<Ts...>::Contains(Entity entity) const
    {
        if(const AutoComponentGroup* group = Get<AutoComponentGroup>(entity); group)
            return group->Contains<T>();

        return false;
    }

    template<class... Ts>
    template<class T>
    auto Registry<Ts...>::Get(Entity entity) -> T*
    {
        auto& storage = GetStorageFor<T>();
        auto poolIndex = storage.sparseArray.at(EntityUtils::Index(entity));
        if(poolIndex != EntityTraits::NullIndex)
            return &storage.componentPool.at(poolIndex);

        auto pos = std::ranges::find_if(storage.stagingPool, [entity](const auto& pair)
        {
            return pair.entity == entity;
        });

        return pos == storage.stagingPool.end() ? nullptr : &pos->component;
    }

    template<class... Ts>
    template<class T>
    auto Registry<Ts...>::Get(Entity entity) const -> const T*
    {
        const auto& storage = GetStorageFor<T>();
        auto poolIndex = storage.sparseArray.at(EntityUtils::Index(entity));
        if(poolIndex != EntityTraits::NullIndex)
            return &storage.componentPool.at(poolIndex);

        const auto pos = std::ranges::find_if(storage.stagingPool, [entity](const auto& pair)
        {
            return pair.entity == entity;
        });

        return pos == storage.stagingPool.end() ? nullptr : &pos->component;
    }

    template<class... Ts>
    template<std::derived_from<AutoComponent> T>
    auto Registry<Ts...>::Get(Entity entity) -> T*
    {
        if(AutoComponentGroup* group = Get<AutoComponentGroup>(entity); group)
            return group->Get<T>();
        
        return nullptr;
    }

    template<class... Ts>
    template<std::derived_from<AutoComponent> T>
    auto Registry<Ts...>::Get(Entity entity) const -> const T*
    {
        if(const AutoComponentGroup* group = Get<AutoComponentGroup>(entity); group)
            return group->Get<T>();
        
        return nullptr;
    }

    template<class... Ts>
    template<std::same_as<Entity> T>
    auto Registry<Ts...>::ViewAll() -> std::span<Entity>
    {
        return std::span<Entity>{m_active};
    }

    template<class... Ts>
    template<class T>
    auto Registry<Ts...>::ViewAll() -> std::span<T>
    {
        return std::span<T>{GetStorageFor<T>().componentPool};
    }

    template<class... Ts>
    template<class T>
    auto Registry<Ts...>::ViewAll() const -> std::span<const T>
    {
        return std::span<const T>{GetStorageFor<T>().componentPool};
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
    template<class T>
    void Registry<Ts...>::CommitStagedComponents()
    {
        auto& storage = GetStorageFor<T>();

        for(auto& [entity, component] : storage.stagingPool)
        {
            storage.componentPool.push_back(std::move(component));
            auto sparseIndex = EntityUtils::Index(entity);
            storage.entityPool.push_back(sparseIndex);
            storage.sparseArray.at(sparseIndex) = storage.componentPool.size() - 1;
        }

        storage.stagingPool.clear();
    }

    template<class... Ts>
    void Registry<Ts...>::CommitStagedChanges()
    {
        for(auto entity : m_toRemove)
        {
            Get<AutoComponentGroup>(entity)->SendOnDestroy();
            (Remove<Ts>(entity), ...);
            m_handleManager.ReclaimHandle(entity);
        }

        m_toRemove.clear();

        // can just insert whole vector?
        for(auto entity : m_toAdd)
        {
            m_active.push_back(entity);
        }

        m_toAdd.clear();

        (CommitStagedComponents<Ts>(), ...);
    }

    template<class... Ts>
    void Registry<Ts...>::Clear()
    {
        for(auto entity : m_toAdd)
            Get<AutoComponentGroup>(entity)->SendOnDestroy();
        for(auto entity : m_toRemove)
            Get<AutoComponentGroup>(entity)->SendOnDestroy();
        for(auto entity : m_active)
            Get<AutoComponentGroup>(entity)->SendOnDestroy();

        m_active.clear();
        m_active.shrink_to_fit();
        m_toAdd.clear();
        m_toAdd.shrink_to_fit();
        m_toRemove.clear();
        m_toRemove.shrink_to_fit();

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