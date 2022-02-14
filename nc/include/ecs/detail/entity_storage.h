#pragma once

#include "HandleManager.h"

#include <algorithm>
#include <vector>

namespace nc::detail
{
    class entity_storage
    {
        public:
            using iterator = std::vector<Entity>::iterator;

            entity_storage()
                : m_entities{},
                  m_toRemove{},
                  m_persistent{},
                  m_handleManager{}
            {
            }

            auto add(const EntityInfo& info) -> Entity
            {
                auto handle = m_handleManager.GenerateNewHandle(info.layer, info.flags);
                m_entities.push_back(handle);
                if(handle.IsPersistent())
                    m_persistent.push_back(handle);

                return handle;
            }

            void remove(Entity entity)
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

            bool contains(Entity entity) const
            {
                return m_entities.cend() != std::ranges::find(m_entities, entity);
            }

            void clear()
            {
                m_toRemove.shrink_to_fit();
                m_persistent.shrink_to_fit();
                m_entities = m_persistent;
                m_handleManager.Reset(m_persistent);
            }

            auto get_staged_removals() const -> const std::vector<Entity>& { return m_toRemove; }

            auto view() -> std::span<Entity> { return m_entities; }
            auto view() const -> std::span<const Entity> { return m_entities; }

            void commit_staged_changes()
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