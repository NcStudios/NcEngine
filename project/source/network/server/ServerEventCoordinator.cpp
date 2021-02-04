#include "ServerEventCoordinator.h"
#include "ServerConnectionManager.h"
#include "network/PacketBuffer.h"
#include "../Packet.h"
#include "enet/enet.h"

namespace project::network
{
    using namespace nc::net;

    ServerEventCoordinator::ServerEventCoordinator(ServerConnectionManager* connectionManager,
                                                   std::function<void(PacketBuffer data, Channel channel, ENetPeer* peer)> sendCallback)
    {
        m_dispatchTable = 
        {
            {
                PacketType::ClientSendName, [connectionManager](ENetEvent* event)
                {
                    Packet::ClientSendName packet;
                    FromByteArray(event->packet->data + PacketTypeSize, &packet);
                    connectionManager->FinalizeConnection(event->peer, packet.name);
                }
            },
            {
                PacketType::SpawnPrefab, [sendCallback](ENetEvent* event)
                {
                    static NetworkHandle nextFreeHandle = 1u;
                    Packet::SpawnPrefab packet;
                    FromByteArray(event->packet->data + PacketTypeSize, &packet);
                    packet.networkHandle = nextFreeHandle++;
                    sendCallback(ToPacketBuffer(&packet), Channel::Reliable, nullptr);
                }
            },
            {
                PacketType::DestroyPrefab, [sendCallback](ENetEvent* event)
                {
                    Packet::DestroyPrefab packet;
                    FromByteArray(event->packet->data + PacketTypeSize, &packet);
                    sendCallback(ToPacketBuffer(&packet), Channel::Reliable, nullptr);
                }
            },
            {
                PacketType::TestNetworkDispatcher, [sendCallback](ENetEvent* event)
                {
                    Packet::TestNetworkDispatcher packet;
                    FromByteArray(event->packet->data + PacketTypeSize, &packet);
                    sendCallback(ToPacketBuffer(&packet), Channel::Reliable, nullptr);
                }
            }
        };
    }

    void ServerEventCoordinator::Dispatch(ENetEvent* event)
    {
        const auto packetType = GetPacketTypeFromBuffer(event->packet->data);
        m_dispatchTable.at(packetType)(event);
    }
}