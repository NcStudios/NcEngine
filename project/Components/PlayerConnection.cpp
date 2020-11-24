#include "PlayerConnection.h"

#include "input/Input.h"

using namespace nc;

namespace project
{
    PlayerConnection::PlayerConnection(std::string name)
        : m_playerName{ std::move(name) }
    {
        m_client.Connect("192.168.0.16", m_playerName);
    }

    void PlayerConnection::FrameUpdate(float dt)
    {
        (void)dt;
        m_client.Poll();
    }
}