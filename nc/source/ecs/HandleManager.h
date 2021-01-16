#pragma once

#include "EntityHandle.h"

namespace nc::ecs
{
    class HandleManager
    {
        static const EntityHandle::Handle_t initialHandle = 1u;

        public:
            HandleManager() : m_current{initialHandle} {}
            EntityHandle GenerateNewHandle() {return EntityHandle{m_current++}; }
            EntityHandle GetCurrent() const { return EntityHandle{m_current}; }
            void Reset() { m_current = initialHandle; }
        
        private:
            EntityHandle::Handle_t m_current;
    };
}