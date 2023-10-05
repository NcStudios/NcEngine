#pragma once

#include "Tag.h"
#include "Transform.h"
#include "detail/EntityStorage.h"
#include "detail/HandleManager.h"
#include "detail/PerComponentStorage.h"
#include "detail/FreeComponentGroup.h"

namespace nc
{
class Registry;

/** @deprecated Use NcEngine::Registry() instead. */
auto ActiveRegistry() -> Registry*;

/** @brief Requirements for views over the registry. */
template<class T>
concept Viewable = PooledComponent<T> || std::same_as<T, Entity>;

/**
 * @brief Storage orchestrator for all Entities and Components.
 */
class Registry
{
    using index_type = Entity::index_type;

    public:
        Registry(size_t maxEntities);
        Registry(Registry&&) = default;
        Registry& operator=(Registry&&) = default;
        ~Registry() = default;
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
        void RegisterComponentType(ComponentHandler<T> handler = {});

        template<PooledComponent T>
        auto Handler() -> ComponentHandler<T>&;

        template<PooledComponent T> requires StoragePolicy<T>::EnableOnAddCallbacks
        auto OnAdd() -> Signal<T&>&;

        template<PooledComponent T> requires StoragePolicy<T>::EnableOnRemoveCallbacks
        auto OnRemove() -> Signal<Entity>&;

        /** Engine Functions */
        template<PooledComponent T>
        auto StorageFor() -> ecs::detail::PerComponentStorage<T>*;

        template<PooledComponent T>
        auto StorageFor() const -> const ecs::detail::PerComponentStorage<T>*;

        template<std::same_as<Entity> T>
        auto StorageFor() -> ecs::detail::EntityStorage*;

        template<std::same_as<Entity> T>
        auto StorageFor() const -> const ecs::detail::EntityStorage*;

        void CommitStagedChanges();
        void Clear();
        auto GetAllComponentsOn(Entity entity) -> std::vector<AnyComponent>;

    private:
        std::vector<std::unique_ptr<ecs::detail::PerComponentStorageBase>> m_registeredStorage;
        ecs::detail::EntityStorage m_entities;
        size_t m_maxEntities;

        void RemoveEntityWithoutNotifyingParent(Entity entity);

        template<PooledComponent T>
        inline static ecs::detail::PerComponentStorage<T>* m_typedStoragePtr = nullptr;
};

template<PooledComponent T>
auto Registry::StorageFor() -> ecs::detail::PerComponentStorage<T>*
{
    NC_ASSERT(m_typedStoragePtr<T>, "Cannot access unregistered component type");
    return m_typedStoragePtr<T>;
}

template<PooledComponent T>
auto Registry::StorageFor() const -> const ecs::detail::PerComponentStorage<T>*
{
    NC_ASSERT(m_typedStoragePtr<T>, "Cannot access unregistered component type");
    return m_typedStoragePtr<T>;
}

template<std::same_as<Entity> T>
auto Registry::StorageFor() -> ecs::detail::EntityStorage*
{
    return &m_entities;
}

template<std::same_as<Entity> T>
auto Registry::StorageFor() const -> const ecs::detail::EntityStorage*
{
    return &m_entities;
}

template<std::same_as<Entity> T>
auto Registry::Add(EntityInfo info) -> Entity
{
    auto handle = m_entities.Add(info);
    Add<Transform>(handle, info.position, info.rotation, info.scale, info.parent);
    Add<ecs::detail::FreeComponentGroup>(handle);
    Add<Tag>(handle, std::move(info.tag));
    return handle;
}

template<std::derived_from<ComponentBase> T, class... Args>
auto Registry::Add(Entity entity, Args&&... args) -> T*
{
    NC_ASSERT(Contains<Entity>(entity), "Bad Entity");
    if constexpr(std::derived_from<T, FreeComponent>)
    {
        auto* group = Get<ecs::detail::FreeComponentGroup>(entity);
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
        auto* group = StorageFor<ecs::detail::FreeComponentGroup>()->Get(entity);
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
        const auto* group = Get<ecs::detail::FreeComponentGroup>(entity);
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
        auto* group = Get<ecs::detail::FreeComponentGroup>(entity);
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
        auto* group = Get<ecs::detail::FreeComponentGroup>(entity);
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
void Registry::RegisterComponentType(ComponentHandler<T> handlers)
{
    if(m_typedStoragePtr<T>)
        return;

    auto storage = std::make_unique<ecs::detail::PerComponentStorage<T>>(m_maxEntities, std::move(handlers));
    m_typedStoragePtr<T> = storage.get();
    m_registeredStorage.push_back(std::move(storage));
}

template<PooledComponent T>
auto Registry::Handler() -> ComponentHandler<T>&
{
    return StorageFor<T>()->Handler();
}

template<PooledComponent T> requires StoragePolicy<T>::EnableOnAddCallbacks
auto Registry::OnAdd() -> Signal<T&>&
{
    return StorageFor<T>()->OnAdd();
}

template<PooledComponent T> requires StoragePolicy<T>::EnableOnRemoveCallbacks
auto Registry::OnRemove() -> Signal<Entity>&
{
    return StorageFor<T>()->OnRemove();
}

inline void Registry::CommitStagedChanges()
{
    const auto& toRemove = m_entities.StagedRemovals();
    for(auto& storage : m_registeredStorage)
        storage->CommitStagedComponents(toRemove);

    m_entities.CommitStagedChanges();
}

/** @todo Temporary include for POC - Should add view/iterator support for this. */
inline auto Registry::GetAllComponentsOn(Entity entity) -> std::vector<AnyComponent>
{
    auto out = std::vector<AnyComponent>{};
    if (!Contains<Entity>(entity))
    {
        return out;
    }

    for (auto& storage : m_registeredStorage)
    {
        if (auto&& any = storage->GetAsAnyComponent(entity))
        {
            out.push_back(std::move(any));
        }
    }

    return out;
}
} // namespace nc
