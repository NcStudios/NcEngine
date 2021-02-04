#include "ClientEventCoordinator.h"
#include "NetworkPrefabManager.h"
#include "debug/Utils.h"
#include "component/NetworkDispatcher.h"
#include "enet/enet.h"
#include "../Packet.h"

namespace project::network
{
    using namespace nc::net;

    ClientEventCoordinator::ClientEventCoordinator(NetworkPrefabManager* networkPrefabManager)
    {
        m_dispatchTable = 
        {
            {
                PacketType::SpawnPrefab, [networkPrefabManager](ENetEvent* event)
                {
                    Packet::SpawnPrefab packet;
                    FromByteArray(event->packet->data + PacketTypeSize, &packet);
                    IF_THROW(!networkPrefabManager, "ClientEventCoordinator::HandleSpawnPrefab - networkPrefabManager is not set");
                    networkPrefabManager->Spawn(packet);
                }
            },
            {
                PacketType::DestroyPrefab, [networkPrefabManager](ENetEvent* event)
                {
                    Packet::DestroyPrefab packet;
                    FromByteArray(event->packet->data + PacketTypeSize, &packet);
                    networkPrefabManager->Destroy(packet.networkHandle);
                }
            },
            {
                PacketType::TestNetworkDispatcher, [networkPrefabManager](ENetEvent* event)
                {
                    auto [packetType, networkHandle] = ReadPacketHeader(event->packet->data);
                    auto dispatcher = networkPrefabManager->GetDispatcher(networkHandle);
                    dispatcher->Dispatch(packetType, event->packet->data + PacketHeaderSize);
                }
            }
        };
    }

    void ClientEventCoordinator::Dispatch(ENetEvent* event)
    {
        const auto packetType = GetPacketTypeFromBuffer(event->packet->data);
        m_dispatchTable.at(packetType)(event);
    }
}