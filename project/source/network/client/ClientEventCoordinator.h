#pragma once

#include "../Packet.h"

struct _ENetEvent;
typedef _ENetEvent ENetEvent;

namespace project::network
{
    class ClientEventCoordinator
    {
        public:
            void Dispatch(ENetEvent* event);

        private:
            template<PacketType packet_t>
            void HandleEvent(ENetEvent* event);
    };

    template<>
    void ClientEventCoordinator::HandleEvent<PacketType::Test>(ENetEvent* event);
}