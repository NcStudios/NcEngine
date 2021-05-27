#pragma once

#include "entity/HandleUtils.h"

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

            EntityHandle GenerateNewHandle(HandleTraits::layer_type layer, HandleTraits::flags_type flags)
            {
                if(m_freeHandles.empty())
                    return EntityHandle{HandleUtils::Join(m_nextIndex++, 0u, layer, flags)};
                
                auto out = m_freeHandles.back();
                m_freeHandles.pop_back();
                return HandleUtils::Recycle(out, layer, flags);
            }

            void ReclaimHandle(EntityHandle handle)
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
            std::vector<EntityHandle> m_freeHandles;
            HandleTraits::index_type m_nextIndex;
    };
}