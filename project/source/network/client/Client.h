#pragma once
#include "nc/source/net/Endpoint.h"
#include "ClientEventCoordinator.h"

#include <string>

namespace project::network
{
    class Client : public nc::net::Endpoint
    {
        public:
            Client();
            ~Client();

            void Poll(uint32_t timeout = 0u) override;
            void Send(nc::net::PacketBuffer data, nc::net::Channel channel, ENetPeer* peer) override;

            bool Connect(const char* hostAddress, std::string& playerName);

        private:
            ENetPeer* m_server;
            ClientEventCoordinator m_coordinator;
    };
}