#pragma once

#include "HandleManager.h"
#include "PerComponentStorage.h"
#include "component/FreeComponentGroup.h"
#include "component/Tag.h"
#include "component/Transform.h"

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

            /** PooledComponent Functions */
            template<PooledComponent T, class... Args>
            auto Add(Entity entity, Args&&... args) -> T*;

            template<PooledComponent T>
            void Remove(Entity entity);

            template<PooledComponent T>
            bool Contains(Entity entity) const;

            template<PooledComponent T>
            auto Get(Entity entity) -> T*;

            template<PooledComponent T>
            auto Get(Entity entity) const -> const T*;

            template<PooledComponent T>
            auto ViewAll() -> std::span<T>;

            template<PooledComponent T>
            auto ViewAll() const -> std::span<const T>;

            template<PooledComponent T, PooledComponent U>
            auto ViewGroup() -> std::pair<std::span<T>, std::span<U>>;

            template<PooledComponent T, class Predicate>
            void Sort(Predicate&& comparesLessThan);

            template<PooledComponent T>
            void ReserveHeadroom(size_t additionalRequiredCount);

            /** FreeComponent Functions */
            template<std::derived_from<FreeComponent> T, class ... Args>
            auto Add(Entity entity, Args&& ... args) -> T*;

            template<std::derived_from<FreeComponent> T>
            void Remove(Entity entity);

            template<std::derived_from<FreeComponent> T>
            bool Contains(Entity entity) const;

            template<std::derived_from<FreeComponent> T>
            auto Get(Entity entity) -> T*;

            template<std::derived_from<FreeComponent> T>
            auto Get(Entity entity) const -> const T*;

            /** System Functions */
            template<PooledComponent T>
            void RegisterComponentType();

            template<PooledComponent T>
            void RegisterOnAddCallback(SystemCallbacks<T>::on_add_type func);

            template<PooledComponent T>
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

            template<PooledComponent T>
            inline static PerComponentStorage<T>* m_typedStoragePtr = nullptr;

            template<PooledComponent T>
            auto StorageFor() -> PerComponentStorage<T>*;

            template<PooledComponent T>
            auto StorageFor() const -> const PerComponentStorage<T>*;
    };

    template<std::same_as<Entity> T>
    auto Registry::Add(EntityInfo info) -> Entity
    {
        auto handle = m_handleManager.GenerateNewHandle(info.layer, info.flags);
        m_toAdd.push_back(handle);
        Add<Transform>(handle, info.position, info.rotation, info.scale, info.parent);
        Add<FreeComponentGroup>(handle);
        Add<Tag>(handle, std::move(info.tag));
        return handle;
    }

    template<PooledComponent T>
    auto Registry::StorageFor() -> PerComponentStorage<T>*
    {
        IF_THROW(!m_typedStoragePtr<T>, "Cannot access unregistered component type");
        return m_typedStoragePtr<T>;
    }

    template<PooledComponent T>
    auto Registry::StorageFor() const -> const PerComponentStorage<T>*
    {
        IF_THROW(!m_typedStoragePtr<T>, "Cannot access unregistered component type");
        return m_typedStoragePtr<T>;
    }

    template<PooledComponent T, class... Args>
    auto Registry::Add(Entity entity, Args&&... args) -> T*
    {
        IF_THROW(!Contains<Entity>(entity), "Bad Entity");
        return StorageFor<T>()->Add(entity, std::forward<Args>(args)...);
    }

    template<std::derived_from<FreeComponent> T, class... Args>
    auto Registry::Add(Entity entity, Args&&... args) -> T*
    {
        IF_THROW(!Contains<Entity>(entity), "Bad Entity");
        FreeComponentGroup* group = Get<FreeComponentGroup>(entity);
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

    template<PooledComponent T>
    void Registry::Remove(Entity entity)
    {
        IF_THROW(!Contains<Entity>(entity), "Bad Entity");
        StorageFor<T>()->Remove(entity);
    }

    template<std::derived_from<FreeComponent> T>
    void Registry::Remove(Entity entity)
    {
        IF_THROW(!Contains<Entity>(entity), "Bad Entity");
        FreeComponentGroup* group = StorageFor<FreeComponentGroup>()->Get(entity);
        group->Remove<T>();
    }

    template<std::same_as<Entity> T>
    bool Registry::Contains(Entity entity) const
    {
        return (m_active.cend() != std::ranges::find(m_active, entity)) ||
               (m_toAdd.cend() != std::ranges::find(m_toAdd, entity));
    }

    template<PooledComponent T>
    bool Registry::Contains(Entity entity) const
    {
        IF_THROW(!Contains<Entity>(entity), "Bad Entity");
        return StorageFor<T>()->Contains(entity);
    }

    template<std::derived_from<FreeComponent> T>
    bool Registry::Contains(Entity entity) const
    {
        IF_THROW(!Contains<Entity>(entity), "Bad Entity");
        const FreeComponentGroup* group = Get<FreeComponentGroup>(entity);
        return group->Contains<T>();
    }

    template<PooledComponent T>
    auto Registry::Get(Entity entity) -> T*
    {
        return StorageFor<T>()->Get(entity);
    }

    template<PooledComponent T>
    auto Registry::Get(Entity entity) const -> const T*
    {
        return StorageFor<T>()->Get(entity);
    }

    template<std::derived_from<FreeComponent> T>
    auto Registry::Get(Entity entity) -> T*
    {
        FreeComponentGroup* group = Get<FreeComponentGroup>(entity);
        return group ? group->Get<T>() : nullptr;
    }

    template<std::derived_from<FreeComponent> T>
    auto Registry::Get(Entity entity) const -> const T*
    {
        const FreeComponentGroup* group = Get<FreeComponentGroup>(entity);
        return group ? group->Get<T>() : nullptr;
    }

    template<std::same_as<Entity> T>
    auto Registry::ViewAll() -> std::span<Entity>
    {
        return std::span<Entity>{m_active};
    }

    template<PooledComponent T>
    auto Registry::ViewAll() -> std::span<T>
    {
        return StorageFor<T>()->ViewAll();
    }

    template<PooledComponent T>
    auto Registry::ViewAll() const -> std::span<const T>
    {
        return StorageFor<T>()->ViewAll();
    }

    template<PooledComponent T, PooledComponent U>
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

    template<PooledComponent T, class Predicate>
    void Registry::Sort(Predicate&& comparesLessThan)
    {
        StorageFor<T>()->Sort(std::forward<Predicate>(comparesLessThan));
    }

    template<PooledComponent T>
    void Registry::ReserveHeadroom(size_t additionalRequiredCount)
    {
        StorageFor<T>()->ReserveHeadroom();
    }

    template<PooledComponent T>
    void Registry::RegisterComponentType()
    {
        if(m_typedStoragePtr<T>)
            return;

        auto storage = std::make_unique<PerComponentStorage<T>>(m_maxEntities);
        m_typedStoragePtr<T> = storage.get();
        m_registeredStorage.push_back(std::move(storage));
    }

    template<PooledComponent T>
    void Registry::RegisterOnAddCallback(typename SystemCallbacks<T>::on_add_type func)
    {
        static_assert(StoragePolicy<T>::requires_on_add_callback::value, "Cannot register an OnAdd callback unless specified in the StoragePolicy");
        StorageFor<T>()->RegisterOnAddCallback(std::move(func));
    }

    template<PooledComponent T>
    void Registry::RegisterOnRemoveCallback(typename SystemCallbacks<T>::on_remove_type func)
    {
        static_assert(StoragePolicy<T>::requires_on_remove_callback::value, "Cannot register an OnRemove callback unless specified in the StoragePolicy");
        StorageFor<T>()->RegisterOnRemoveCallback(std::move(func));
    }

    inline void Registry::CommitStagedChanges()
    {
        m_handleManager.ReclaimHandles(m_toRemove);

        for(auto& storage : m_registeredStorage)
            storage->CommitStagedComponents(m_toRemove);

        m_toRemove.clear();
        m_active.reserve(m_active.size() + m_toAdd.size());
        m_active.insert(m_active.end(), m_toAdd.cbegin(), m_toAdd.cend());
        m_toAdd.clear();
    }
}