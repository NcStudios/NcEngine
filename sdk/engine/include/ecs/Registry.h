#pragma once

#include "detail/EntityStorage.h"
#include "detail/HandleManager.h"
#include "detail/PerComponentStorage.h"
#include "component/FreeComponentGroup.h"
#include "component/Tag.h"
#include "component/Transform.h"

namespace nc
{
    class Registry;

    /** @deprecated Use NcEngine::Registry() instead. */
    auto ActiveRegistry() -> Registry*;

    /** @brief Requirements for views over the registry. */
    template<class T>
    concept Viewable = PooledComponent<T> || std::same_as<T, Entity>;

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

            /** Component Functions */
            template<std::derived_from<ComponentBase> T, class... Args>
            auto Add(Entity entity, Args&&... args) -> T*;

            template<std::derived_from<ComponentBase> T>
            void Remove(Entity entity);

            template<std::derived_from<ComponentBase> T>
            bool Contains(Entity entity) const;

            template<std::derived_from<ComponentBase> T>
            auto Get(Entity entity) -> T*;

            template<std::derived_from<ComponentBase> T>
            auto Get(Entity entity) const -> const T*;

            /** PooledComponent Specific Functions */
            template<PooledComponent T, PooledComponent U>
            auto ViewGroup() -> std::pair<std::span<T>, std::span<U>>;

            template<PooledComponent T, std::predicate<const T&, const T&> Predicate>
            void Sort(Predicate&& comparesLessThan);

            template<PooledComponent T>
            void ReserveHeadroom(size_t additionalRequiredCount);

            template<PooledComponent T>
            void RegisterComponentType();

            template<PooledComponent T>
            void RegisterOnAddCallback(detail::SystemCallbacks<T>::on_add_type func);

            template<PooledComponent T>
            void RegisterOnRemoveCallback(detail::SystemCallbacks<T>::on_remove_type func);

            /** Engine Functions */
            template<PooledComponent T>
            auto StorageFor() -> detail::PerComponentStorage<T>*;

            template<PooledComponent T>
            auto StorageFor() const -> const detail::PerComponentStorage<T>*;

            template<std::same_as<Entity> T>
            auto StorageFor() -> detail::EntityStorage*;

            template<std::same_as<Entity> T>
            auto StorageFor() const -> const detail::EntityStorage*;

            void CommitStagedChanges();
            void VerifyCallbacks();
            void Clear();

        private:
            std::vector<std::unique_ptr<detail::PerComponentStorageBase>> m_registeredStorage;
            detail::EntityStorage m_entities;
            size_t m_maxEntities;

            void RemoveEntityWithoutNotifyingParent(Entity entity);

            template<PooledComponent T>
            inline static detail::PerComponentStorage<T>* m_typedStoragePtr = nullptr;
    };

    template<PooledComponent T>
    auto Registry::StorageFor() -> detail::PerComponentStorage<T>*
    {
        NC_ASSERT(m_typedStoragePtr<T>, "Cannot access unregistered component type");
        return m_typedStoragePtr<T>;
    }

    template<PooledComponent T>
    auto Registry::StorageFor() const -> const detail::PerComponentStorage<T>*
    {
        NC_ASSERT(m_typedStoragePtr<T>, "Cannot access unregistered component type");
        return m_typedStoragePtr<T>;
    }

    template<std::same_as<Entity> T>
    auto Registry::StorageFor() -> detail::EntityStorage*
    {
        return &m_entities;
    }

    template<std::same_as<Entity> T>
    auto Registry::StorageFor() const -> const detail::EntityStorage*
    {
        return &m_entities;
    }

    template<std::same_as<Entity> T>
    auto Registry::Add(EntityInfo info) -> Entity
    {
        auto handle = m_entities.Add(info);
        Add<Transform>(handle, info.position, info.rotation, info.scale, info.parent);
        Add<FreeComponentGroup>(handle);
        Add<Tag>(handle, std::move(info.tag));
        return handle;
    }

    template<std::derived_from<ComponentBase> T, class... Args>
    auto Registry::Add(Entity entity, Args&&... args) -> T*
    {
        NC_ASSERT(Contains<Entity>(entity), "Bad Entity");
        if constexpr(std::derived_from<T, FreeComponent>)
        {
            FreeComponentGroup* group = Get<FreeComponentGroup>(entity);
            return group->Add<T>(entity, std::forward<Args>(args)...);
        }
        else
        {
            return StorageFor<T>()->Add(entity, std::forward<Args>(args)...);
        }
    }

    template<std::same_as<Entity> T>
    void Registry::Remove(Entity entity)
    {
        NC_ASSERT(Contains<Entity>(entity), "Bad Entity");
        auto* transform = Get<Transform>(entity);
        transform->SetParent(Entity::Null());

        for(auto child : transform->Children())
            RemoveEntityWithoutNotifyingParent(child);

        m_entities.Remove(entity);
    }

    template<std::derived_from<ComponentBase> T>
    void Registry::Remove(Entity entity)
    {
        NC_ASSERT(Contains<Entity>(entity), "Bad Entity");
        if constexpr(std::derived_from<T, FreeComponent>)
        {
            FreeComponentGroup* group = StorageFor<FreeComponentGroup>()->Get(entity);
            group->Remove<T>();
        }
        else
        {
            StorageFor<T>()->Remove(entity);
        }
    }

    template<std::same_as<Entity> T>
    bool Registry::Contains(Entity entity) const
    {
        return m_entities.Contains(entity);
    }

    template<std::derived_from<ComponentBase> T>
    bool Registry::Contains(Entity entity) const
    {
        NC_ASSERT(Contains<Entity>(entity), "Bad Entity");
        if constexpr(std::derived_from<T, FreeComponent>)
        {
            const FreeComponentGroup* group = Get<FreeComponentGroup>(entity);
            return group->Contains<T>();
        }
        else
        {
            return StorageFor<T>()->Contains(entity);
        }
    }

    template<std::derived_from<ComponentBase> T>
    auto Registry::Get(Entity entity) -> T*
    {
        if constexpr(std::derived_from<T, FreeComponent>)
        {
            FreeComponentGroup* group = Get<FreeComponentGroup>(entity);
            return group ? group->Get<T>() : nullptr;
        }
        else
        {
            return StorageFor<T>()->Get(entity);
        }
    }

    template<std::derived_from<ComponentBase> T>
    auto Registry::Get(Entity entity) const -> const T*
    {
        if constexpr(std::derived_from<T, FreeComponent>)
        {
            FreeComponentGroup* group = Get<FreeComponentGroup>(entity);
            return group ? group->Get<T>() : nullptr;
        }
        else
        {
            return StorageFor<T>()->Get(entity);
        }
    }

    template<PooledComponent T, PooledComponent U>
    auto Registry::ViewGroup() -> std::pair<std::span<T>, std::span<U>>
    {
        auto& referenceStorage = *StorageFor<T>();
        auto& targetStorage = *StorageFor<U>();
        auto& referenceEntities = referenceStorage.EntityPool();
        auto& targetSparseArray = targetStorage.SparseArray();
        auto& targetEntities = targetStorage.EntityPool();

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

        auto& referenceComponents = referenceStorage.ComponentPool();
        auto& targetComponents = targetStorage.ComponentPool();

        if(referenceComponents.size() < current || targetComponents.size() < current)
            throw NcError("ViewGroup - Invalid size");

        auto sharedRangeSize = current > end ? current : end;

        return std::pair<std::span<T>, std::span<U>>
        {
            std::span<T>{referenceComponents.begin(), sharedRangeSize},
            std::span<U>{targetComponents.begin(), sharedRangeSize}
        };
    }

    template<PooledComponent T, std::predicate<const T&, const T&> Predicate>
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

        auto storage = std::make_unique<detail::PerComponentStorage<T>>(m_maxEntities);
        m_typedStoragePtr<T> = storage.get();
        m_registeredStorage.push_back(std::move(storage));
    }

    template<PooledComponent T>
    void Registry::RegisterOnAddCallback(typename detail::SystemCallbacks<T>::on_add_type func)
    {
        static_assert(storage_policy<T>::requires_on_add_callback, "Cannot register an OnAdd callback unless specified by storage_policy<T>");
        StorageFor<T>()->RegisterOnAddCallback(std::move(func));
    }

    template<PooledComponent T>
    void Registry::RegisterOnRemoveCallback(typename detail::SystemCallbacks<T>::on_remove_type func)
    {
        static_assert(storage_policy<T>::requires_on_remove_callback, "Cannot register an OnRemove callback unless specified by storage_policy<T>");
        StorageFor<T>()->RegisterOnRemoveCallback(std::move(func));
    }

    inline void Registry::CommitStagedChanges()
    {
        const auto& toRemove = m_entities.StagedRemovals();
        for(auto& storage : m_registeredStorage)
            storage->CommitStagedComponents(toRemove);

        m_entities.CommitStagedChanges();
    }
}