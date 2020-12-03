#include "Network.h"
#include "DebugUtils.h"

namespace nc::net
{
    std::function<void(PacketBuffer)> Network::ServerCommandCallback = nullptr;

    void Network::BindServerCommandCallback(std::function<void(PacketBuffer)> callback)
    {
        Network::ServerCommandCallback = callback;
    }

    void Network::ServerCommand(PacketBuffer buffer)
    {
        IF_THROW(!Network::ServerCommandCallback, "Network::ServerCommandCallback is not set");
        Network::ServerCommandCallback(buffer);
    }
}