#pragma once

#include "enet/enet.h"
#include "PacketBuffer.h"

#include <stdexcept>

namespace nc::net
{
    const auto CHANNEL_LIMIT = 2u;

    enum class Channel : uint8_t
    {
        Reliable = 0u,
        Unreliable = 1u
    };

    class Endpoint
    {
        public:
            Endpoint() { if(enet_initialize()) throw std::runtime_error("Failed to initialize Enet"); }
            virtual ~Endpoint() { enet_deinitialize(); }

            virtual void Poll(uint32_t timeout = 0u) = 0;
            virtual void Send(PacketBuffer data, Channel channel, ENetPeer* peer) = 0;
        
        protected:
            ENetHost* host;
            ENetAddress address;
    };
}