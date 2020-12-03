#pragma once

#include "source/net/PacketBuffer.h"

#include <functional>

namespace nc::net
{
    class Network
    {
        public:
            static void ServerCommand(PacketBuffer buffer);
            static void BindServerCommandCallback(std::function<void(PacketBuffer)> callback);

        private:
            static std::function<void(PacketBuffer)> ServerCommandCallback;
    };
}