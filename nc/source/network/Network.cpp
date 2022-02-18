#include "Network.h"
#include "debug/Utils.h"

namespace nc::net
{
    std::function<void(const PacketBuffer&)> g_SendHandler = nullptr;

    void BindServerCommandSendHandler(std::function<void(const PacketBuffer&)> callback)
    {
        g_SendHandler = callback;
    }

    void ServerCommand(const PacketBuffer& buffer)
    {
        NC_ASSERT(g_SendHandler, "net::ServerCommandHandler is not set");
        g_SendHandler(buffer);
    }
}