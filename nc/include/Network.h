#pragma once

#include "net/PacketBuffer.h"

#include <functional>

namespace nc::net
{
    class Network
    {
        public:
            static void ServerCommand(const PacketBuffer& buffer);
            static void BindServerCommandCallback(std::function<void(const PacketBuffer&)> callback);

        private:
            static std::function<void(const PacketBuffer&)> ServerCommandCallback;
    };
}