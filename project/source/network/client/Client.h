#pragma once
#include "network/Endpoint.h"
#include "ClientEventCoordinator.h"
#include "NetworkPrefabManager.h"

#include <string>

namespace project::network
{
    class Client : public nc::net::Endpoint
    {
        public:
            Client(std::string serverIP);
            ~Client();

            void Poll(uint32_t timeout = 0u) override;
            void Send(const nc::net::PacketBuffer& data, nc::net::Channel channel, ENetPeer* peer) override;
            void SendToServer(const nc::net::PacketBuffer& data);

            bool Connect(std::string& playerName);

        private:
            ENetPeer* m_server;
            NetworkPrefabManager m_networkPrefabManager;
            ClientEventCoordinator m_coordinator;
            std::string m_serverIP;
    };
}