#pragma once

#include "ncengine/ecs/Ecs.h"
#include "ncengine/ecs/detail/FreeComponentGroup.h"
#include "ncengine/ecs/detail/HandleManager.h"
#include "ncengine/type/StableAddress.h"

namespace nc
{
class Registry;

/** @deprecated Use NcEngine::Registry() instead. */
auto ActiveRegistry() -> Registry*;

/** @brief Requirements for views over the registry. */
template<class T>
concept Viewable = PooledComponent<T> || std::same_as<T, Entity>;

/** @brief Legacy interface over ecs storage and access.
 *  @deprecated This is implemented in terms of nc::ecs::EcsInterface. New code should
 *              prefer using EcsInterface directly over Registry. */
class Registry : public StableAddress
{
    using index_type = Entity::index_type;

    public:
        explicit Registry(ecs::ComponentRegistry& impl);

        /** Entity Functions */
        template<std::same_as<Entity> T>
        auto Add(EntityInfo info) -> Entity
        {
            return m_ecs.Emplace<Entity>(std::move(info));
        }

        template<std::same_as<Entity> T>
        void Remove(Entity entity)
        {
            m_ecs.Remove<Entity>(entity);
        }

        template<std::same_as<Entity> T>
        bool Contains(Entity entity) const
        {
            return m_ecs.Contains<Entity>(entity);
        }

        /** PooledComponent Specific Functions */
        template<PooledComponent T, class... Args>
        auto Add(Entity entity, Args&&... args) -> T*
        {
            return &m_ecs.Emplace<T>(entity, std::forward<Args>(args)...);
        }

        template<PooledComponent T>
        void Remove(Entity entity)
        {
            m_ecs.Remove<T>(entity);
        }

        template<PooledComponent T>
        bool Contains(Entity entity) const
        {
            return m_ecs.Contains<T>(entity);
        }

        template<PooledComponent T>
        auto Get(Entity entity) -> T*
        {
            return &m_ecs.Get<T>(entity);
        }

        template<PooledComponent T>
        auto Get(Entity entity) const -> const T*
        {
            return &m_ecs.Get<T>(entity);
        }

        template<PooledComponent T>
        auto GetParent(const T* component) const -> Entity
        {
            return m_ecs.GetParent<T>(component);
        }

        template<PooledComponent T, std::predicate<const T&, const T&> Predicate>
        void Sort(Predicate&& comparesLessThan)
        {
            m_impl->GetPool<T>().Sort(std::forward<Predicate>(comparesLessThan));
        }

        template<PooledComponent T>
        void Reserve(size_t additionalRequiredCount)
        {
            m_impl->GetPool<T>().Reserve(additionalRequiredCount);
        }

        // TODO: get manager?
        template<PooledComponent T>
        auto Handler() -> ComponentHandler<T>&
        {
            return m_impl->GetPool<T>()->Handler();
        }

        template<PooledComponent T>
            requires StoragePolicy<T>::EnableOnAddCallbacks
        auto OnAdd() -> Signal<T&>&
        {
            return m_impl->GetPool<T>().OnAdd();
        }

        template<PooledComponent T>
            requires StoragePolicy<T>::EnableOnCommitCallbacks
        auto OnCommit() -> Signal<T&>&
        {
            return m_impl->GetPool<T>().OnCommit();
        }

        template<PooledComponent T>
            requires StoragePolicy<T>::EnableOnRemoveCallbacks
        auto OnRemove() -> Signal<Entity>&
        {
            return m_impl->GetPool<T>().OnRemove();
        }

        /** FreeComponent Specific Functions */
        template<std::derived_from<FreeComponent> T, class... Args>
        auto Add(Entity entity, Args&&... args) -> T*
        {
            return &m_ecs.Emplace<T>(entity, std::forward<Args>(args)...);
        }

        template<std::derived_from<FreeComponent> T>
        void Remove(Entity entity)
        {
            m_ecs.Remove<T>(entity);
        }

        template<std::derived_from<FreeComponent> T>
        bool Contains(Entity entity) const
        {
            return m_ecs.Contains<T>(entity);
        }

        template<std::derived_from<FreeComponent> T>
        auto Get(Entity entity) -> T*
        {
            return &m_ecs.Get<T>(entity);
        }

        template<std::derived_from<FreeComponent> T>
        auto Get(Entity entity) const -> const T*
        {
            return &m_ecs.Get<T>(entity);
        }

        /** Engine Functions */
        template<PooledComponent T>
        auto StorageFor() -> ecs::ComponentPool<T>*
        {
            return &m_impl->GetPool<T>();
        }

        template<PooledComponent T>
        auto StorageFor() const -> const ecs::ComponentPool<T>*
        {
            return &m_impl->GetPool<T>();
        }

        template<std::same_as<Entity> T>
        auto StorageFor() -> ecs::EntityPool*
        {
            return &m_impl->GetPool<Entity>();
        }

        template<std::same_as<Entity> T>
        auto StorageFor() const -> const ecs::EntityPool*
        {
            return &m_impl->GetPool<Entity>();
        }

        auto GetBasePools()
        {
            return m_impl->GetComponentPools();
        }

        void CommitStagedChanges()
        {
            m_impl->CommitPendingChanges();
        }

        void Clear()
        {
            m_impl->ClearSceneData();
        }

        void Reset()
        {
            m_impl->Clear();
        }

        auto GetImpl() -> ecs::ComponentRegistry&
        {
            return *m_impl;
        }

        auto GetEcs() -> ecs::Ecs
        {
            return m_ecs;
        }

        // Workaround: Convenience func to get ecs in scenes
        auto GetEcs() -> ecs::Ecs
        {
            return m_ecs;
        }

    private:
        ecs::ComponentRegistry* m_impl;
        ecs::Ecs m_ecs;
};
} // namespace nc
