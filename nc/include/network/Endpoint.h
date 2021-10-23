#pragma once

#include "enet/enet.h"
#include "NetworkDetails.h"
#include "PacketBuffer.h"
#include "debug/NcError.h"

namespace nc::net
{
    class Endpoint
    {
        public:
            Endpoint() { if(enet_initialize()) throw NcError("Failed to initialize Enet"); }
            virtual ~Endpoint() { enet_deinitialize(); }

            virtual void Poll(uint32_t timeout = 0u) = 0;
            virtual void Send(const PacketBuffer& data, Channel channel, ENetPeer* peer) = 0;
        
        protected:
            ENetHost* host;
            ENetAddress address;
    };
}