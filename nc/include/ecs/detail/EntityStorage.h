#pragma once

#include "HandleManager.h"

#include <algorithm>
#include <vector>

namespace nc::detail
{
    class EntityStorage
    {
        public:
            EntityStorage()
                : m_entities{},
                  m_toRemove{},
                  m_persistent{},
                  m_handleManager{}
            {
            }

            auto Add(const EntityInfo& info) -> Entity
            {
                auto handle = m_handleManager.GenerateNewHandle(info.layer, info.flags);
                m_entities.push_back(handle);
                if(handle.IsPersistent())
                    m_persistent.push_back(handle);

                return handle;
            }

            void Remove(Entity entity)
            {
                auto pos = std::ranges::find(m_entities, entity);
                *pos = m_entities.back();
                m_entities.pop_back();
                m_toRemove.push_back(entity);

                if(entity.IsPersistent())
                {
                    auto pos = std::ranges::find(m_persistent, entity);
                    *pos = m_persistent.back();
                    m_persistent.pop_back();
                }
            }

            bool Contains(Entity entity) const
            {
                return m_entities.cend() != std::ranges::find(m_entities, entity);
            }

            void Clear()
            {
                m_toRemove.shrink_to_fit();
                m_persistent.shrink_to_fit();
                m_entities = m_persistent;
                m_handleManager.Reset(m_persistent);
            }

            auto GetStagedRemovals() const -> const std::vector<Entity>& { return m_toRemove; }

            auto View() -> std::span<Entity> { return m_entities; }
            auto View() const -> std::span<const Entity> { return m_entities; }

            void CommitStagedChanges()
            {
                if(m_toRemove.empty()) return;
                m_handleManager.ReclaimHandles(m_toRemove);
                m_toRemove.clear();
            }

        private:
            std::vector<Entity> m_entities;
            std::vector<Entity> m_toRemove;
            std::vector<Entity> m_persistent;
            HandleManager m_handleManager;
    };
}