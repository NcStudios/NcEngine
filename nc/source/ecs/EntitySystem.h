#pragma once

#include "HandleManager.h"
#include "alloc/PoolAdapter.h"
#include "entity/Entity.h"
#include "entity/EntityInfo.h"

#include <concepts>
#include <span>
#include <vector>

namespace nc::ecs
{
    class EntitySystem
    {
        public:
            EntitySystem(size_t count);

            EntityHandle Add(const EntityInfo& info);
            void Remove(EntityHandle handle);
            bool Contains(EntityHandle handle);
            Entity* Get(EntityHandle handle);
            Entity* Get(const std::string& tag);
            std::span<Entity*> GetActiveEntities();
            void Clear();

            template<std::invocable<const Entity&> UnaryFunc>
            void CommitRemovals(UnaryFunc func);

        private:
            bool IsActive(EntityHandle handle)
            {
                return m_activePool.Contains([handle](auto* e) { return e->Handle == handle; });
            }

            bool IsStaged(EntityHandle handle)
            {
                return m_toAddStaging.cend() != std::ranges::find_if(m_toAddStaging, [handle](const auto& e) { return e.Handle == handle; });
            }

            alloc::PoolAdapter<Entity> m_activePool;
            std::vector<Entity> m_toAddStaging;
            std::vector<Entity> m_toRemove;
            HandleManager m_handle;
    };

    inline EntitySystem::EntitySystem(size_t count)
        : m_activePool{count},
          m_toAddStaging{},
          m_toRemove{}
    {
    }

    inline EntityHandle EntitySystem::Add(const EntityInfo& info)
    {
        /** @todo layer still handled old way */
        auto handle = m_handle.GenerateNewHandle(0u, info.flags);
        m_toAddStaging.emplace_back(handle, std::move(info.tag), info.layer);
        return handle;
    }

    inline void EntitySystem::Remove(EntityHandle handle)
    {
        // what if in add staging?
        m_toRemove.push_back(m_activePool.Extract([handle](auto* e) { return e->Handle == handle; }));
    }

    inline bool EntitySystem::Contains(EntityHandle handle)
    {
        return IsActive(handle) || IsStaged(handle);
    }

    inline Entity* EntitySystem::Get(EntityHandle handle)
    {
        auto* ptr = m_activePool.Get([handle](auto* e) { return e->Handle == handle; });

        if(ptr)
            return ptr;

        auto pos = std::ranges::find_if(m_toAddStaging, [handle](auto& e) { return e.Handle == handle; });
        return pos == m_toAddStaging.end() ? nullptr : &(*pos);
    }

    inline Entity* EntitySystem::Get(const std::string& tag)
    {
        auto* ptr = m_activePool.Get([&tag](auto* e) { return e->Tag == tag; });

        if(ptr)
            return ptr;

        auto pos = std::ranges::find_if(m_toAddStaging, [&tag](auto& e) { return e.Tag == tag; });
        return pos == m_toAddStaging.end() ? nullptr : &(*pos);
    }

    inline std::span<Entity*> EntitySystem::GetActiveEntities()
    {
        return m_activePool.GetActiveRange();
    }

    inline void EntitySystem::Clear()
    {
        for(auto& entity : m_toAddStaging)
            entity.SendOnDestroy();

        for(auto* entity : m_activePool.GetActiveRange())
            entity->SendOnDestroy();

        for(auto& entity : m_toRemove)
            entity.SendOnDestroy();

        m_activePool.Clear();
        m_toAddStaging.clear();
        m_toRemove.clear();
        m_handle.Reset();
    }

    template<std::invocable<const Entity&> UnaryFunc>
    void EntitySystem::CommitRemovals(UnaryFunc func)
    {
        for(auto& entity : m_toRemove)
        {
            entity.SendOnDestroy();
            func(entity);
        }

        m_toRemove.clear();

        for(auto& entity : m_toAddStaging)
        {
            m_activePool.Insert(std::move(entity));
        }

        m_toAddStaging.clear();
    }
} // namespace nc::ecs