#pragma once

#include "AutoComponentGroup.h"
#include "HandleManager.h"
#include "PerComponentStorage.h"
#include "Tag.h"
#include "Transform.h"

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
 *   - Buffering
 *   - Groups
 *   - Remove multiple?
 */

namespace nc::ecs
{
    template<Component... Ts>
    struct RegistryTypeList
    {
        using storage_type = std::tuple<PerComponentStorage<Ts>...>;

        static storage_type Create(size_t maxEntities)
        {
            return storage_type{PerComponentStorage<Ts>(maxEntities)...};
        }
    };

    template<class TypeList>
    class Registry
    {
        using storage_type = TypeList::storage_type;
        using index_type = Entity::index_type;

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
            void Remove(Entity entity);

            template<std::same_as<Entity> T>
            bool Contains(Entity entity) const;

            template<std::same_as<Entity> T>
            auto ViewAll() -> std::span<Entity>;

            /** Component Functions */
            template<Component T, class... Args>
            auto Add(Entity entity, Args&&... args) -> T*;

            template<Component T>
            void Remove(Entity entity);

            template<Component T>
            bool Contains(Entity entity) const;

            template<Component T>
            auto Get(Entity entity) -> T*;

            template<Component T>
            auto Get(Entity entity) const -> const T*;

            template<Component T>
            auto ViewAll() -> std::span<T>;

            template<Component T>
            auto ViewAll() const -> std::span<const T>;

            template<Component T, Component U>
            auto ViewGroup() -> std::pair<std::span<T>, std::span<U>>;

            template<Component T>
            void ReserveHeadroom(size_t additionalRequiredCount);

            /** AutoComponent Functions */
            template<std::derived_from<AutoComponent> T, class ... Args>
            auto Add(Entity entity, Args&& ... args) -> T*;

            template<std::derived_from<AutoComponent> T>
            void Remove(Entity entity);

            template<std::derived_from<AutoComponent> T>
            bool Contains(Entity entity) const;

            template<std::derived_from<AutoComponent> T>
            auto Get(Entity entity) -> T*;

            template<std::derived_from<AutoComponent> T>
            auto Get(Entity entity) const -> const T*;

            /** System Functions */
            template<Component T>
            void RegisterOnAddCallback(SystemCallbacks<T>::on_add_type func);

            template<Component T>
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

            void RemoveEntityWithoutNotifyingParent(Entity entity);

            template<Component T>
            auto GetStorageFor() -> PerComponentStorage<T>& { return std::get<PerComponentStorage<T>>(m_storage); }

            template<Component T>
            auto GetStorageFor() const -> const PerComponentStorage<T>& { return std::get<PerComponentStorage<T>>(m_storage); }
    };

    template<class TypeList>
    Registry<TypeList>::Registry(size_t maxEntities)
        : m_storage{TypeList::Create(maxEntities)},
          m_active{},
          m_toAdd{},
          m_toRemove{},
          m_handleManager{}
    {
    }

    template<class TypeList>
    template<std::same_as<Entity> T>
    auto Registry<TypeList>::Add(EntityInfo info) -> Entity
    {
        auto handle = m_handleManager.GenerateNewHandle(info.layer, info.flags);
        m_toAdd.push_back(handle);
        Add<Transform>(handle, info.position, info.rotation, info.scale, info.parent);
        Add<AutoComponentGroup>(handle);
        Add<Tag>(handle, std::move(info.tag));
        return handle;
    }

    template<class TypeList>
    template<Component T, class... Args>
    auto Registry<TypeList>::Add(Entity entity, Args&&... args) -> T*
    {
        IF_THROW(!Contains<Entity>(entity), "Bad Entity");
        return GetStorageFor<T>().Add(entity, std::forward<Args>(args)...);
    }

    template<class TypeList>
    template<std::derived_from<AutoComponent> T, class... Args>
    auto Registry<TypeList>::Add(Entity entity, Args&&... args) -> T*
    {
        IF_THROW(!Contains<Entity>(entity), "Bad Entity");
        AutoComponentGroup* group = Get<AutoComponentGroup>(entity);
        return group->Add<T>(entity, std::forward<Args>(args)...);
    }

    template<class TypeList>
    template<std::same_as<Entity> T>
    void Registry<TypeList>::Remove(Entity entity)
    {
        IF_THROW(!Contains<Entity>(entity), "Bad Entity");
        auto* transform = Get<Transform>(entity);
        transform->SetParent(Entity::Null());
        
        for(auto child : transform->GetChildren())
            RemoveEntityWithoutNotifyingParent(child);

        auto pos = std::ranges::find(m_active, entity);
        *pos = m_active.back();
        m_active.pop_back();
        m_toRemove.push_back(entity);
    }

    template<class TypeList>
    void Registry<TypeList>::RemoveEntityWithoutNotifyingParent(Entity entity)
    {
        IF_THROW(!Contains<Entity>(entity), "Bad Entity");
        auto* transform = Get<Transform>(entity);
        for(auto child : transform->GetChildren())
            RemoveEntityWithoutNotifyingParent(child);
        
        auto pos = std::ranges::find(m_active, entity);
        *pos = m_active.back();
        m_active.pop_back();
        m_toRemove.push_back(entity);
    }

    template<class TypeList>
    template<Component T>
    void Registry<TypeList>::Remove(Entity entity)
    {
        IF_THROW(!Contains<Entity>(entity), "Bad Entity");
        GetStorageFor<T>().Remove(entity);
    }

    template<class TypeList>
    template<std::derived_from<AutoComponent> T>
    void Registry<TypeList>::Remove(Entity entity)
    {
        IF_THROW(!Contains<Entity>(entity), "Bad Entity");
        AutoComponentGroup* group = GetStorageFor<AutoComponentGroup>().Get(entity);
        group->Remove<T>();
    }

    template<class TypeList>
    template<std::same_as<Entity> T>
    bool Registry<TypeList>::Contains(Entity entity) const
    {
        return (m_active.cend() != std::ranges::find(m_active, entity)) ||
               (m_toAdd.cend() != std::ranges::find(m_toAdd, entity));
    }

    template<class TypeList>
    template<Component T>
    bool Registry<TypeList>::Contains(Entity entity) const
    {
        IF_THROW(!Contains<Entity>(entity), "Bad Entity");
        return GetStorageFor<T>().Contains(entity);
    }

    template<class TypeList>
    template<std::derived_from<AutoComponent> T>
    bool Registry<TypeList>::Contains(Entity entity) const
    {
        IF_THROW(!Contains<Entity>(entity), "Bad Entity");
        const AutoComponentGroup* group = Get<AutoComponentGroup>(entity);
        return group->Contains<T>();
    }

    template<class TypeList>
    template<Component T>
    auto Registry<TypeList>::Get(Entity entity) -> T*
    {
        return GetStorageFor<T>().Get(entity);
    }

    template<class TypeList>
    template<Component T>
    auto Registry<TypeList>::Get(Entity entity) const -> const T*
    {
        return GetStorageFor<T>().Get(entity);
    }

    template<class TypeList>
    template<std::derived_from<AutoComponent> T>
    auto Registry<TypeList>::Get(Entity entity) -> T*
    {
        AutoComponentGroup* group = Get<AutoComponentGroup>(entity);
        return group ? group->Get<T>() : nullptr;
    }

    template<class TypeList>
    template<std::derived_from<AutoComponent> T>
    auto Registry<TypeList>::Get(Entity entity) const -> const T*
    {
        const AutoComponentGroup* group = Get<AutoComponentGroup>(entity);
        return group ? group->Get<T>() : nullptr;
    }

    template<class TypeList>
    template<std::same_as<Entity> T>
    auto Registry<TypeList>::ViewAll() -> std::span<Entity>
    {
        return std::span<Entity>{m_active};
    }

    template<class TypeList>
    template<Component T>
    auto Registry<TypeList>::ViewAll() -> std::span<T>
    {
        return GetStorageFor<T>().ViewAll();
    }

    template<class TypeList>
    template<Component T>
    auto Registry<TypeList>::ViewAll() const -> std::span<const T>
    {
        return GetStorageFor<T>().ViewAll();
    }

    template<class TypeList>
    template<Component T, Component U>
    auto Registry<TypeList>::ViewGroup() -> std::pair<std::span<T>, std::span<U>>
    {
        auto& referenceStorage = GetStorageFor<T>();
        auto& targetStorage = GetStorageFor<U>();
        auto& referenceEntities = referenceStorage.GetEntityPool();
        auto& targetSparseArray = targetStorage.GetSparseArray();
        auto& targetEntities = targetStorage.GetEntityPool();

        size_t referenceSize = referenceEntities.size();
        if(referenceSize == 0u || targetEntities.size() == 0u)
            return{};

        size_t current = 0u, end = referenceSize - 1;

        while(current <= end)
        {
            auto entityIndex = referenceEntities.at(current);
            auto indexInTarget = targetSparseArray.at(entityIndex);

            if(indexInTarget != Entity::NullIndex) // Entity has both components
            {
                if(indexInTarget != current)
                {
                    auto swapEntity = targetEntities.at(current);
                    targetStorage.Swap(entityIndex, swapEntity);
                }

                ++current;
            }
            else // Entity does not have target component
            {
                auto swapEntity = referenceEntities.at(end);
                referenceStorage.Swap(entityIndex, swapEntity);
                --end;
            }
        }

        auto& referenceComponents = referenceStorage.GetComponentPool();
        auto& targetComponents = targetStorage.GetComponentPool();

        // changes
        if(referenceComponents.size() < current || targetComponents.size() < current)
            throw std::runtime_error("ViewGroup - Invalid size");

        auto sharedRangeSize = current > end ? current : end;

        return std::pair<std::span<T>, std::span<U>>
        {
            std::span<T>{referenceComponents.begin(), sharedRangeSize},
            std::span<U>{targetComponents.begin(), sharedRangeSize}
        };
    }

    template<class TypeList>
    template<Component T>
    void Registry<TypeList>::ReserveHeadroom(size_t additionalRequiredCount)
    {
        GetStorageFor<T>().ReserveHeadroom();
    }

    template<class TypeList>
    template<Component T>
    void Registry<TypeList>::RegisterOnAddCallback(typename SystemCallbacks<T>::on_add_type func)
    {
        static_assert(StoragePolicy<T>::requires_on_add_callback::value, "Cannot register an OnAdd callback unless specified in the StoragePolicy");
        GetStorageFor<T>().RegisterOnAddCallback(std::move(func));
    }

    template<class TypeList>
    template<Component T>
    void Registry<TypeList>::RegisterOnRemoveCallback(typename SystemCallbacks<T>::on_remove_type func)
    {
        static_assert(StoragePolicy<T>::requires_on_remove_callback::value, "Cannot register an OnRemove callback unless specified in the StoragePolicy");
        GetStorageFor<T>().RegisterOnRemoveCallback(std::move(func));
    }

    template<class TypeList>
    void Registry<TypeList>::CommitStagedChanges()
    {
        for(auto entity : m_toRemove)
        {
            Get<AutoComponentGroup>(entity)->SendOnDestroy();
            std::apply([entity](auto&&... storage) { (storage.TryRemove(entity), ...); }, m_storage);
            m_handleManager.ReclaimHandle(entity);
        }

        m_toRemove.clear();
        m_active.reserve(m_active.size() + m_toAdd.size());
        m_active.insert(m_active.end(), m_toAdd.cbegin(), m_toAdd.cend());
        m_toAdd.clear();
        std::apply([](auto&&... storage) { (storage.CommitStagedComponents(), ...); }, m_storage);
    }

    template<class TypeList>
    void Registry<TypeList>::Clear()
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

        m_handleManager.Reset();
    }

    template<class TypeList>
    void Registry<TypeList>::VerifyCallbacks()
    {
        std::apply([](auto&&... storage) { (storage.VerifyCallbacks(), ...); }, m_storage);
    }
}