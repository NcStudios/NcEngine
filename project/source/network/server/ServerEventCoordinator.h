#pragma once

#include "../Packet.h"

struct _ENetEvent;
typedef _ENetEvent ENetEvent;

namespace project::network
{
    class ServerConnectionManager;

    class ServerEventCoordinator
    {
        public:
            ServerEventCoordinator(ServerConnectionManager* connectionManager);

            void Dispatch(ENetEvent* eventPacket);

        private:
            ServerConnectionManager* m_connections;

            template<PacketType packet_t>
            void HandleEvent(ENetEvent* packet);
    };

    template<>
    void ServerEventCoordinator::HandleEvent<PacketType::ClientSendName>(ENetEvent* event);
}