#include "ServerEventCoordinator.h"
#include "ServerConnectionManager.h"
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
                    PacketClientSendName packet(event->packet->data + PacketTypeSize);
                    connectionManager->FinalizeConnection(event->peer, packet.name);
                }
            },
            {
                PacketType::SpawnPrefab, [sendCallback](ENetEvent* event)
                {
                    static NetworkHandle nextFreeHandle = 1u;
                    PacketSpawnPrefab packet(event->packet->data + PacketTypeSize);
                    packet.networkHandle = nextFreeHandle++;
                    sendCallback(packet.ToPacketBuffer(), Channel::Reliable, nullptr);
                }
            },
            {
                PacketType::DestroyPrefab, [sendCallback](ENetEvent* event)
                {
                    PacketDestroyPrefab packet(event->packet->data + PacketTypeSize);
                    sendCallback(packet.ToPacketBuffer(), Channel::Reliable, nullptr);
                }
            },
            {
                PacketType::TestNetworkDispatcher, [sendCallback](ENetEvent* event)
                {
                    PacketTestNetworkDispatcher packet(event->packet->data + PacketTypeSize);
                    sendCallback(packet.ToPacketBuffer(), Channel::Reliable, nullptr);
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