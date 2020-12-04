#include "Network.h"
#include "DebugUtils.h"

namespace nc::net
{
    std::function<void(const PacketBuffer&)> Network::ServerCommandCallback = nullptr;

    void Network::BindServerCommandCallback(std::function<void(const PacketBuffer&)> callback)
    {
        Network::ServerCommandCallback = callback;
    }

    void Network::ServerCommand(const PacketBuffer& buffer)
    {
        IF_THROW(!Network::ServerCommandCallback, "Network::ServerCommandCallback is not set");
        Network::ServerCommandCallback(buffer);
    }
}