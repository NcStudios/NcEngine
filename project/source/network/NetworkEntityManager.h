#pragma once

#include "Ecs.h"

#include <unordered_map>

namespace project::network
{
    using NetworkHandle = uint32_t;

    class NetworkEntitymanager
    {
        public:
            NetworkHandle Add(nc::Entity* entity)
            {
                m_entities[m_nextHandle] = std::pair{m_nextHandle, entity};
                return m_nextHandle++;
            }

            bool Remove(NetworkHandle handle)
            {
                return m_entities.erase(handle);
            }

            bool Exists(NetworkHandle handle)
            {
                return (m_entities.find(handle) == m_entities.end()) ? false : true;
            }

            nc::Entity* Get(NetworkHandle handle)
            {
                if(!Exists(handle))
                {
                    throw std::runtime_error("No entity exists for network handle");
                }
                return m_entities.at(handle);
            }

        private:
            std::unordered_map<NetworkHandle, nc::Entity*> m_entities;
            uint32_t m_nextHandle = 0u;
    };
}