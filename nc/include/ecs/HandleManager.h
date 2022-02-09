#pragma once

#include "Entity.h"

#include <algorithm>
#include <iterator>
#include <span>
#include <vector>

namespace nc
{
    class HandleManager
    {
        public:
            HandleManager()
                : m_freeHandles{},
                  m_nextIndex{0u}
            {
            }

            Entity GenerateNewHandle(Entity::layer_type layer, Entity::flags_type flags)
            {
                if(m_freeHandles.empty())
                    return Entity{m_nextIndex++, layer, flags};

                auto index = m_freeHandles.back();
                m_freeHandles.pop_back();
                return Entity{index, layer, flags};
            }

            void ReclaimHandle(Entity handle)
            {
                m_freeHandles.push_back(handle.Index());
            }

            void ReclaimHandles(std::span<Entity> handles)
            {
                m_freeHandles.reserve(m_freeHandles.capacity() + handles.size());
                std::ranges::transform(handles.begin(), handles.end(), std::back_inserter(m_freeHandles), [](auto entity)
                {
                    return entity.Index();
                });
            }

            void Reset(const std::vector<Entity>& persistentEntities)
            {
                m_freeHandles.clear();
                m_freeHandles.shrink_to_fit();
                m_nextIndex = 0u;

                for(auto entity : persistentEntities)
                {
                    auto index = entity.Index();
                    if(index == m_nextIndex)
                    {
                        ++m_nextIndex;
                    }
                    else if(index < m_nextIndex)
                    {
                        auto pos = std::ranges::find(m_freeHandles, index);
                        if(pos != m_freeHandles.end())
                        {
                            *pos = m_freeHandles.back();
                            m_freeHandles.pop_back();
                        }
                    }
                    else
                    {
                        while(m_nextIndex < index)
                        {
                            m_freeHandles.push_back(m_nextIndex++);
                        }

                        ++m_nextIndex;
                    }
                }
            }
        
        private:
            std::vector<Entity::index_type> m_freeHandles;
            Entity::index_type m_nextIndex;
    };
}