#pragma once

#include "Entity.h"

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
                
                auto out = m_freeHandles.back();
                m_freeHandles.pop_back();
                return Entity{out.Index(), layer, flags};
            }

            void ReclaimHandle(Entity handle)
            {
                m_freeHandles.push_back(handle);
            }

            void Reset()
            {
                m_freeHandles.clear();
                m_freeHandles.shrink_to_fit();
                m_nextIndex = 0u;
            }
        
        private:
            std::vector<Entity> m_freeHandles;
            Entity::index_type m_nextIndex;
    };
}