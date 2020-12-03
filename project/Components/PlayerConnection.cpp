#include "PlayerConnection.h"
#include "Network.h"
#include "input/Input.h"
#include "project/source/Prefabs.h"
#include "project/source/network/Packet.h"

using namespace nc;

namespace project
{
    PlayerConnection::PlayerConnection(std::string serverIP, std::string name)
        : m_client{std::move(serverIP)},
          m_playerName{std::move(name)}
    {
        m_client.Connect(m_playerName);
    }

    void PlayerConnection::FrameUpdate(float dt)
    {
        (void)dt;
        m_client.Poll();

        if(input::GetKeyDown(input::KeyCode::Space))
        {
            network::PacketSpawnPrefab packet(project::prefab::Resource::Boar, nc::net::NullNetworkHandle, 0.4f, 0.0f, 0.4f, 1.57f, 0, 0);
            nc::net::Network::ServerCommand(packet.ToPacketBuffer());
        }

        if(input::GetKeyDown(input::KeyCode::P))
        {
            network::PacketTestNetworkDispatcher packet(1u, 0u);
            nc::net::Network::ServerCommand(packet.ToPacketBuffer());
        }

        if(input::GetKeyDown(input::KeyCode::D))
        {
            network::PacketDestroyPrefab packet(1u);
            nc::net::Network::ServerCommand(packet.ToPacketBuffer());
        }
    }
}