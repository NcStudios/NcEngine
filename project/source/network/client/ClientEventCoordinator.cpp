#include "ClientEventCoordinator.h"
#include "enet/enet.h"

#include <iostream>

namespace project::network
{
    void ClientEventCoordinator::Dispatch(ENetEvent* event)
    {
        std::cout << "ClientEventCoordinator::Dispatch()\n";

        //this will break when PacketType has > 255 entries
        switch(static_cast<PacketType>(event->packet->data[0]))
        {
            case PacketType::Test:
            {
                HandleEvent<PacketType::Test>(event);
                break;
            }
            default:
                throw std::runtime_error("Unknonwn EventType");
        }
    }

    template<>
    void ClientEventCoordinator::HandleEvent<PacketType::Test>(ENetEvent* event)
    {
        auto headerSize = sizeof(PacketType);
        PacketTest packet(event->packet->data + headerSize, event->packet->dataLength - headerSize);
        packet.Print();
    }
}