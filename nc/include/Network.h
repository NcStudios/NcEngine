#pragma once

#include "net/PacketBuffer.h"

#include <functional>

namespace nc::net
{
    /** Dispatch a server command through the registered handler. */
    void ServerCommand(const PacketBuffer& buffer);
    
    /** Bind a function for sending commands to a server. */
    void BindServerCommandSendHandler(std::function<void(const PacketBuffer&)> callback);
}