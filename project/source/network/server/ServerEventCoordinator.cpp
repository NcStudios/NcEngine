#include "ServerEventCoordinator.h"
#include "ServerConnectionManager.h"
#include "enet/enet.h"

#include <iostream>

namespace project::network
{
    ServerEventCoordinator::ServerEventCoordinator(ServerConnectionManager* connectionManager)
        : m_connections{connectionManager}
    {
    }

    void ServerEventCoordinator::Dispatch(ENetEvent* event)
    {
        std::cout << "ServerEventCoordinator::Dispatch()\n";

        //this will break when PacketType has > 255 entries
        switch(static_cast<PacketType>(event->packet->data[0]))
        {
            case PacketType::ClientSendName:
            {
                HandleEvent<PacketType::ClientSendName>(event);
                break;
            }
            default:
                throw std::runtime_error("Unknonwn EventType");
        }
    }

    template<>
    void ServerEventCoordinator::HandleEvent<PacketType::ClientSendName>(ENetEvent* event)
    {
        auto headerSize = sizeof(PacketType);
        PacketClientSendName packet(event->packet->data + headerSize, event->packet->dataLength - headerSize);
        packet.Print();
        m_connections->FinalizeConnection(event->peer, packet.name);
    }
}