#pragma once

#include "HandleManager.h"
#include "PerComponentStorage.h"
#include "component/AutoComponentGroup.h"
#include "component/Tag.h"
#include "component/Transform.h"

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

namespace nc
{
    class Registry;

    /** @deprecated Use NcEngine::Registry() instead. */
    auto ActiveRegistry() -> Registry*;

    class Registry
    {
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
            void RegisterComponentType();

            template<Component T>
            void RegisterOnAddCallback(SystemCallbacks<T>::on_add_type func);

            template<Component T>
            void RegisterOnRemoveCallback(SystemCallbacks<T>::on_remove_type func);

            /** Engine Functions */
            void CommitStagedChanges();
            void VerifyCallbacks();
            void Clear();

        private:
            std::vector<std::unique_ptr<PerComponentStorageBase>> m_registeredStorage;
            std::vector<Entity> m_active;
            std::vector<Entity> m_toAdd;
            std::vector<Entity> m_toRemove;
            HandleManager m_handleManager;
            size_t m_maxEntities;

            void RemoveEntityWithoutNotifyingParent(Entity entity);

            template<Component T>
            inline static PerComponentStorage<T>* m_typedStoragePtr = nullptr;

            template<Component T>
            auto StorageFor() -> PerComponentStorage<T>*;

            template<Component T>
            auto StorageFor() const -> const PerComponentStorage<T>*;
    };

    template<std::same_as<Entity> T>
    auto Registry::Add(EntityInfo info) -> Entity
    {
        auto handle = m_handleManager.GenerateNewHandle(info.layer, info.flags);
        m_toAdd.push_back(handle);
        Add<Transform>(handle, info.position, info.rotation, info.scale, info.parent);
        Add<AutoComponentGroup>(handle);
        Add<Tag>(handle, std::move(info.tag));
        return handle;
    }

    template<Component T>
    auto Registry::StorageFor() -> PerComponentStorage<T>*
    {
        IF_THROW(!m_typedStoragePtr<T>, "Cannot access unregistered component type");
        return m_typedStoragePtr<T>;
    }

    template<Component T>
    auto Registry::StorageFor() const -> const PerComponentStorage<T>*
    {
        IF_THROW(!m_typedStoragePtr<T>, "Cannot access unregistered component type");
        return m_typedStoragePtr<T>;
    }

    template<Component T, class... Args>
    auto Registry::Add(Entity entity, Args&&... args) -> T*
    {
        IF_THROW(!Contains<Entity>(entity), "Bad Entity");
        return StorageFor<T>()->Add(entity, std::forward<Args>(args)...);
    }

    template<std::derived_from<AutoComponent> T, class... Args>
    auto Registry::Add(Entity entity, Args&&... args) -> T*
    {
        IF_THROW(!Contains<Entity>(entity), "Bad Entity");
        AutoComponentGroup* group = Get<AutoComponentGroup>(entity);
        return group->Add<T>(entity, std::forward<Args>(args)...);
    }

    template<std::same_as<Entity> T>
    void Registry::Remove(Entity entity)
    {
        IF_THROW(!Contains<Entity>(entity), "Bad Entity");
        auto* transform = Get<Transform>(entity);
        transform->SetParent(Entity::Null());
        
        for(auto child : transform->Children())
            RemoveEntityWithoutNotifyingParent(child);

        auto pos = std::ranges::find(m_active, entity);
        *pos = m_active.back();
        m_active.pop_back();
        m_toRemove.push_back(entity);
    }

    template<Component T>
    void Registry::Remove(Entity entity)
    {
        IF_THROW(!Contains<Entity>(entity), "Bad Entity");
        StorageFor<T>()->Remove(entity);
    }

    template<std::derived_from<AutoComponent> T>
    void Registry::Remove(Entity entity)
    {
        IF_THROW(!Contains<Entity>(entity), "Bad Entity");
        AutoComponentGroup* group = StorageFor<AutoComponentGroup>()->Get(entity);
        group->Remove<T>();
    }

    template<std::same_as<Entity> T>
    bool Registry::Contains(Entity entity) const
    {
        return (m_active.cend() != std::ranges::find(m_active, entity)) ||
               (m_toAdd.cend() != std::ranges::find(m_toAdd, entity));
    }

    template<Component T>
    bool Registry::Contains(Entity entity) const
    {
        IF_THROW(!Contains<Entity>(entity), "Bad Entity");
        return StorageFor<T>()->Contains(entity);
    }

    template<std::derived_from<AutoComponent> T>
    bool Registry::Contains(Entity entity) const
    {
        IF_THROW(!Contains<Entity>(entity), "Bad Entity");
        const AutoComponentGroup* group = Get<AutoComponentGroup>(entity);
        return group->Contains<T>();
    }

    template<Component T>
    auto Registry::Get(Entity entity) -> T*
    {
        return StorageFor<T>()->Get(entity);
    }

    template<Component T>
    auto Registry::Get(Entity entity) const -> const T*
    {
        return StorageFor<T>()->Get(entity);
    }

    template<std::derived_from<AutoComponent> T>
    auto Registry::Get(Entity entity) -> T*
    {
        AutoComponentGroup* group = Get<AutoComponentGroup>(entity);
        return group ? group->Get<T>() : nullptr;
    }

    template<std::derived_from<AutoComponent> T>
    auto Registry::Get(Entity entity) const -> const T*
    {
        const AutoComponentGroup* group = Get<AutoComponentGroup>(entity);
        return group ? group->Get<T>() : nullptr;
    }

    template<std::same_as<Entity> T>
    auto Registry::ViewAll() -> std::span<Entity>
    {
        return std::span<Entity>{m_active};
    }

    template<Component T>
    auto Registry::ViewAll() -> std::span<T>
    {
        return StorageFor<T>()->ViewAll();
    }

    template<Component T>
    auto Registry::ViewAll() const -> std::span<const T>
    {
        return StorageFor<T>()->ViewAll();
    }

    template<Component T, Component U>
    auto Registry::ViewGroup() -> std::pair<std::span<T>, std::span<U>>
    {
        auto& referenceStorage = *StorageFor<T>();
        auto& targetStorage = *StorageFor<U>();
        auto& referenceEntities = referenceStorage.GetEntityPool();
        auto& targetSparseArray = targetStorage.GetSparseArray();
        auto& targetEntities = targetStorage.GetEntityPool();

        size_t referenceSize = referenceEntities.size();
        if(referenceSize == 0u || targetEntities.size() == 0u)
            return{};

        size_t current = 0u, end = referenceSize - 1;

        while(current <= end)
        {
            auto entityIndex = referenceEntities.at(current).Index();
            auto indexInTarget = targetSparseArray.at(entityIndex);

            if(indexInTarget != Entity::NullIndex) // Entity has both components
            {
                if(indexInTarget != current)
                {
                    auto swapEntity = targetEntities.at(current);
                    targetStorage.Swap(entityIndex, swapEntity.Index());
                }

                ++current;
            }
            else // Entity does not have target component
            {
                auto swapEntity = referenceEntities.at(end).Index();
                referenceStorage.Swap(entityIndex, swapEntity);
                --end;
            }
        }

        auto& referenceComponents = referenceStorage.GetComponentPool();
        auto& targetComponents = targetStorage.GetComponentPool();

        if(referenceComponents.size() < current || targetComponents.size() < current)
            throw NcError("ViewGroup - Invalid size");

        auto sharedRangeSize = current > end ? current : end;

        return std::pair<std::span<T>, std::span<U>>
        {
            std::span<T>{referenceComponents.begin(), sharedRangeSize},
            std::span<U>{targetComponents.begin(), sharedRangeSize}
        };
    }

    template<Component T>
    void Registry::ReserveHeadroom(size_t additionalRequiredCount)
    {
        StorageFor<T>()->ReserveHeadroom();
    }

    template<Component T>
    void Registry::RegisterComponentType()
    {
        if(m_typedStoragePtr<T>)
            return;

        auto storage = std::make_unique<PerComponentStorage<T>>(m_maxEntities);
        m_typedStoragePtr<T> = storage.get();
        m_registeredStorage.push_back(std::move(storage));
    }

    template<Component T>
    void Registry::RegisterOnAddCallback(typename SystemCallbacks<T>::on_add_type func)
    {
        static_assert(StoragePolicy<T>::requires_on_add_callback::value, "Cannot register an OnAdd callback unless specified in the StoragePolicy");
        StorageFor<T>()->RegisterOnAddCallback(std::move(func));
    }

    template<Component T>
    void Registry::RegisterOnRemoveCallback(typename SystemCallbacks<T>::on_remove_type func)
    {
        static_assert(StoragePolicy<T>::requires_on_remove_callback::value, "Cannot register an OnRemove callback unless specified in the StoragePolicy");
        StorageFor<T>()->RegisterOnRemoveCallback(std::move(func));
    }

    inline void Registry::CommitStagedChanges()
    {
        for(auto entity : m_toRemove)
        {
            Get<AutoComponentGroup>(entity)->SendOnDestroy();
            m_handleManager.ReclaimHandle(entity);
        }

        for(auto& storage : m_registeredStorage)
            storage->CommitStagedComponents(m_toRemove);

        m_toRemove.clear();
        m_active.reserve(m_active.size() + m_toAdd.size());
        m_active.insert(m_active.end(), m_toAdd.cbegin(), m_toAdd.cend());
        m_toAdd.clear();
    }
}