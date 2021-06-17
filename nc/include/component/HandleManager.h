#pragma once

#include "Entity.h"

#include <vector>

namespace nc::ecs
{
    class HandleManager
    {
        public:
            HandleManager()
                : m_freeHandles{},
                  m_nextIndex{0u}
            {
            }

            Entity GenerateNewHandle(EntityTraits::layer_type layer, EntityTraits::flags_type flags)
            {
                if(m_freeHandles.empty())
                    return Entity{EntityUtils::Join(m_nextIndex++, 0u, layer, flags)};
                
                auto out = m_freeHandles.back();
                m_freeHandles.pop_back();
                return EntityUtils::Recycle(out, layer, flags);
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
            EntityTraits::index_type m_nextIndex;
    };
}