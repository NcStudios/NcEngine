#pragma once

#include "network/NetworkDetails.h"

#include <functional>
#include <unordered_map>

struct _ENetEvent;
typedef _ENetEvent ENetEvent;

struct _ENetPeer;
typedef _ENetPeer ENetPeer;

namespace nc::net { struct PacketBuffer; }

namespace project::network
{
    class ServerConnectionManager;

    class ServerEventCoordinator
    {
        public:
            ServerEventCoordinator(ServerConnectionManager* connectionManager, 
                                   std::function<void(nc::net::PacketBuffer data, nc::net::Channel channel, ENetPeer* peer)> sendCallback);

            void Dispatch(ENetEvent* eventPacket);
        
        private:
            std::unordered_map<nc::net::PacketType, std::function<void(ENetEvent*)>> m_dispatchTable;
    };
}