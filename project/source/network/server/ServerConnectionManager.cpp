#include "ServerConnectionManager.h"
#include "enet/enet.h"

#include <algorithm>
#include <iostream>
#include <stdexcept>

namespace project::network
{
    void ServerConnectionManager::NewConnectionRequest(ENetPeer* peer)
    {
        std::cout << "NewConnectionRequest()\n";
        m_unhandledPeers.push_back(peer);
    }

    void ServerConnectionManager::RemoveConnection(ENetPeer* peer)
    {
        std::cout << "RemoveConnection()\n";

        auto pos = std::find_if(m_peers.begin(), m_peers.end(), [peer](const PeerConnection& connection)
        {
            return connection.peer->connectID == peer->connectID;
        });

        if(pos == m_peers.end())
        {
            throw std::runtime_error("The peer to be removed does not exist");
        }

        std::exchange(*pos, m_peers.back());
        m_peers.pop_back();
    }

    void ServerConnectionManager::FinalizeConnection(ENetPeer* peer, std::string name)
    {
        std::cout << "FinalizeConnection()\n"
                    << "    " << name << " connected\n";

        m_peers.emplace_back(PeerConnection{peer, std::move(name)});

        auto pos = std::find_if(m_unhandledPeers.begin(), m_unhandledPeers.end(), [peer](ENetPeer* current)
        {
            return current->connectID == peer->connectID;
        });

        if(pos == m_unhandledPeers.end())
        {
            throw std::runtime_error("The peer to be removed does not exist");
        }

        std::exchange(*pos, m_unhandledPeers.back());
        m_unhandledPeers.pop_back();
    }
} // end namespace project::network