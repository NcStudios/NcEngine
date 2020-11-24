#pragma once
#include "nc/source/net/Endpoint.h"
#include "ServerEventCoordinator.h"
#include "ServerConnectionManager.h"

#include <vector>

namespace project::network
{
    class Server : public nc::net::Endpoint
    {
        public:
            Server();
            ~Server();

            void Poll(uint32_t timeout = 0u) override;
            void Send(nc::net::PacketBuffer data, nc::net::Channel channel, ENetPeer* peer) override;

        protected:
            void OnEventConnect(ENetEvent* event);
            void OnEventDisconnect(ENetEvent* event);
        
        private:
            ServerConnectionManager m_connections;
            ServerEventCoordinator m_coordinator;
    };
}