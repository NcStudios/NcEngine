#include "NetworkPrefabManager.h"
#include "component/NetworkDispatcher.h"
#include "project/source/Prefabs.h"
#include "../Packet.h"

#include <algorithm>
#include <iostream>

namespace project::network
{
    void NetworkPrefabManager::Spawn(Packet::SpawnPrefab& packet)
    {
        std::cout << "SpawnNetworkPrefab\n";

        auto entityHandle = project::prefab::Create(packet.resource);
        auto dispatcher = nc::Ecs::GetComponent<nc::NetworkDispatcher>(entityHandle);
        IF_THROW(!dispatcher, "NetworkPrefabManager::Spawn - Entity does not have a NetworkDispatcher");
        dispatcher->networkHandle = packet.networkHandle;

        m_data.resources.push_back(packet.resource);
        m_data.entityHandles.push_back(entityHandle);
        m_data.networkHandles.push_back(packet.networkHandle);
        m_data.dispatchers.push_back(dispatcher);

        std::cout << "    EntityHandle:  " << entityHandle << '\n'
                  << "    NetworkHandle: " << packet.networkHandle << '\n'
                  << "    Resource:      " << (int)packet.resource << '\n';
    }
    
    void NetworkPrefabManager::Destroy(nc::net::NetworkHandle networkHandle)
    {
        for(size_t i = 0; i < m_data.networkHandles.size(); ++i)
        {
            if(m_data.networkHandles[i] == networkHandle)
            {
                nc::Ecs::DestroyEntity(m_data.entityHandles[i]);
                std::swap(m_data.resources[i], m_data.resources.back());
                m_data.resources.pop_back();
                std::swap(m_data.entityHandles[i], m_data.entityHandles.back());
                m_data.entityHandles.pop_back();
                std::swap(m_data.networkHandles[i], m_data.networkHandles.back());
                m_data.networkHandles.pop_back();
                std::swap(m_data.dispatchers[i], m_data.dispatchers.back());
                m_data.dispatchers.pop_back();
                return;
            }
        }

        throw std::runtime_error("NetworkPrefabManager::Destroy - Invalid NetworkHandle");
    }
    
    bool NetworkPrefabManager::Exists(nc::net::NetworkHandle handle) const
    {
        auto pos = std::find(m_data.networkHandles.begin(), m_data.networkHandles.end(), handle);
        return pos == m_data.networkHandles.end() ? false : true;
    }
    
    nc::NetworkDispatcher* NetworkPrefabManager::GetDispatcher(nc::net::NetworkHandle networkHandle)
    {
        for(size_t i = 0; i < m_data.networkHandles.size(); ++i)
        {
            if(m_data.networkHandles[i] == networkHandle)
            {
                return m_data.dispatchers[i];
            }
        }

        throw std::runtime_error("NetworkPrefabManager::GetDispatcher - Invalid NetworkHandle");
    }
}