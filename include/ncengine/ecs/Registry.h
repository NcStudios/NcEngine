#pragma once

#include "ComponentPool.h"
#include "Tag.h"
#include "Transform.h"
#include "detail/EntityStorage.h"
#include "detail/FreeComponentGroup.h"
#include "detail/HandleManager.h"
#include "detail/PerComponentStorage.h"
#include "ncengine/type/StableAddress.h"

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
class Registry : public StableAddress
{
    using index_type = Entity::index_type;

    public:
        explicit Registry(size_t maxEntities);

        /** Entity Functions */
        template<std::same_as<Entity> T>
        auto Add(EntityInfo info) -> Entity;

        template<std::same_as<Entity> T>
        void Remove(Entity entity);

        template<std::same_as<Entity> T>
        bool Contains(Entity entity) const;

        /** PooledComponent Specific Functions */
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
        auto GetParent(const T* component) const -> Entity;

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

        /** FreeComponent Specific Functions */
        template<std::derived_from<FreeComponent> T, class... Args>
        auto Add(Entity entity, Args&&... args) -> T*;

        template<std::derived_from<FreeComponent> T>
        void Remove(Entity entity);

        template<std::derived_from<FreeComponent> T>
        bool Contains(Entity entity) const;

        template<std::derived_from<FreeComponent> T>
        auto Get(Entity entity) -> T*;

        template<std::derived_from<FreeComponent> T>
        auto Get(Entity entity) const -> const T*;

        /** Engine Functions */
        template<PooledComponent T>
        auto StorageFor() -> ecs::detail::PerComponentStorage<T>*;

        template<PooledComponent T>
        auto StorageFor() const -> const ecs::detail::PerComponentStorage<T>*;

        template<std::same_as<Entity> T>
        auto StorageFor() -> ecs::detail::EntityStorage*;

        template<std::same_as<Entity> T>
        auto StorageFor() const -> const ecs::detail::EntityStorage*;

        auto GetComponentPools() -> std::span<std::unique_ptr<ecs::ComponentPool>>;
        void CommitStagedChanges();
        void Clear();
        auto GetAllComponentsOn(Entity entity) -> std::vector<AnyComponent>;

    private:
        std::vector<std::unique_ptr<ecs::ComponentPool>> m_registeredStorage;
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

inline auto Registry::GetComponentPools() -> std::span<std::unique_ptr<ecs::ComponentPool>>
{
    return m_registeredStorage;
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

template<PooledComponent T, class... Args>
auto Registry::Add(Entity entity, Args&&... args) -> T*
{
    NC_ASSERT(Contains<Entity>(entity), "Bad Entity");
    return StorageFor<T>()->Add(entity, std::forward<Args>(args)...);
}

template<std::derived_from<FreeComponent> T, class... Args>
auto Registry::Add(Entity entity, Args&&... args) -> T*
{
    NC_ASSERT(Contains<Entity>(entity), "Bad Entity");
    auto* group = Get<ecs::detail::FreeComponentGroup>(entity);
    return group->Add<T>(entity, std::forward<Args>(args)...);
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

template<PooledComponent T>
void Registry::Remove(Entity entity)
{
    NC_ASSERT(Contains<Entity>(entity), "Bad Entity");
    StorageFor<T>()->Remove(entity);
}

template<std::derived_from<FreeComponent> T>
void Registry::Remove(Entity entity)
{
    NC_ASSERT(Contains<Entity>(entity), "Bad Entity");
    auto* group = StorageFor<ecs::detail::FreeComponentGroup>()->Get(entity);
    group->Remove<T>();
}

template<std::same_as<Entity> T>
bool Registry::Contains(Entity entity) const
{
    return m_entities.Contains(entity);
}

template<PooledComponent T>
bool Registry::Contains(Entity entity) const
{
    NC_ASSERT(Contains<Entity>(entity), "Bad Entity");
    return StorageFor<T>()->Contains(entity);
}

template<std::derived_from<FreeComponent> T>
bool Registry::Contains(Entity entity) const
{
    NC_ASSERT(Contains<Entity>(entity), "Bad Entity");
    const auto* group = Get<ecs::detail::FreeComponentGroup>(entity);
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
    auto* group = Get<ecs::detail::FreeComponentGroup>(entity);
    return group ? group->Get<T>() : nullptr;
}

template<std::derived_from<FreeComponent> T>
auto Registry::Get(Entity entity) const -> const T*
{
    auto* group = Get<ecs::detail::FreeComponentGroup>(entity);
    return group ? group->Get<T>() : nullptr;
}

template<PooledComponent T>
auto Registry::GetParent(const T* component) const -> Entity
{
    return StorageFor<T>()->GetParent(component);
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

/** @todo #437 Temporary include for POC - Should add view/iterator support for this. */
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
