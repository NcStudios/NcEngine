#include "PlayerConnection.h"
#include "Network.h"
#include "input/Input.h"
#include "project/source/Prefabs.h"
#include "project/source/network/Packet.h"

using namespace nc;
using namespace project::network;

namespace project
{
    PlayerConnection::PlayerConnection(ComponentHandle handle, EntityHandle parentHandle, std::string serverIP, std::string name)
        : Component(handle, parentHandle),
          m_client{std::move(serverIP)},
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
            Packet::SpawnPrefab packet
            {
                .resource = project::prefab::Resource::Boar,
                .networkHandle = net::NullNetworkHandle,
                .posX = 0.4f, .posY = 0.0f, .posZ = 0.4f,
                .rotX = 1.57f, .rotY = 0.0f, .rotZ = 0.0f
            };
            net::Network::ServerCommand(ToPacketBuffer(&packet));
        }

        if(input::GetKeyDown(input::KeyCode::P))
        {
            Packet::TestNetworkDispatcher packet{.networkHandle = 1u, .value = 0u};
            net::Network::ServerCommand(ToPacketBuffer(&packet));
        }

        if(input::GetKeyDown(input::KeyCode::D))
        {
            Packet::DestroyPrefab packet{.networkHandle = 1u};
            net::Network::ServerCommand(ToPacketBuffer(&packet));
        }
    }
}