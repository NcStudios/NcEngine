#pragma once

#include "net/NetworkDetails.h"

#include <functional>
#include <unordered_map>

struct _ENetEvent;
typedef _ENetEvent ENetEvent;

namespace project::network
{
    class NetworkPrefabManager;

    class ClientEventCoordinator
    {
        public:
            ClientEventCoordinator(NetworkPrefabManager* networkEntityManager);
            void Dispatch(ENetEvent* event);
        private:
            std::unordered_map<nc::net::PacketType, std::function<void(ENetEvent*)>> m_dispatchTable;
    };
}