#pragma once

#include <string>
#include <vector>

struct _ENetPeer;
typedef _ENetPeer ENetPeer;

namespace project::network
{
    class ServerConnectionManager
    {
        public:
            void NewConnectionRequest(ENetPeer* peer);
            void RemoveConnection(ENetPeer* peer);
            void FinalizeConnection(ENetPeer* peer, std::string name);

        private:
            struct PeerConnection
            {
                ENetPeer* peer;
                std::string name; 
            };

            std::vector<PeerConnection> m_peers;
            std::vector<ENetPeer*> m_unhandledPeers;
    };
}