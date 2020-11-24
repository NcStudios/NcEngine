#pragma once

#include "enet/enet.h"

#include <algorithm>
#include <stdexcept>
#include <string>
#include <vector>

#include <iostream>

namespace project::network
{
    struct PeerConnection
    {
        ENetPeer* peer;
        std::string name; 
    };

    class ServerConnectionManager
    {
        public:
            void NewConnectionRequest(ENetPeer* toAdd)
            {
                std::cout << "NewConnectionRequest()\n";
                m_unhandledPeers.push_back(toAdd);
            }

            void RemoveConnection(ENetPeer* toRemove)
            {
                std::cout << "RemoveConnection()\n";

                auto pos = std::find_if(m_peers.begin(), m_peers.end(), [toRemove](const PeerConnection& connection)
                {
                    return connection.peer->connectID == toRemove->connectID;
                });

                if(pos == m_peers.end())
                {
                    throw std::runtime_error("The peer to be removed does not exist");
                }

                std::exchange(*pos, m_peers.back());
                m_peers.pop_back();
            }

            void FinalizeConnection(ENetPeer* peer, std::string name)
            {
                std::cout << "FinalizeConnection()\n";

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

        private:
            std::vector<PeerConnection> m_peers;
            std::vector<ENetPeer*> m_unhandledPeers;
    };
}