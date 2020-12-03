#include "ClientEventCoordinator.h"
#include "NetworkPrefabManager.h"
#include "DebugUtils.h"
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
                    PacketSpawnPrefab packet(event->packet->data + PacketTypeSize);
                    IF_THROW(!networkPrefabManager, "ClientEventCoordinator::HandleSpawnPrefab - networkPrefabManager is not set");
                    networkPrefabManager->Spawn(packet);
                }
            },
            {
                PacketType::DestroyPrefab, [networkPrefabManager](ENetEvent* event)
                {
                    PacketDestroyPrefab packet(event->packet->data + PacketTypeSize);
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