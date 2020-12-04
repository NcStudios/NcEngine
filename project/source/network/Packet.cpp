#include "Packet.h"

#include <cstring>

namespace project::network
{
    nc::net::PacketBuffer ToPacketBuffer(const Packet::ClientSendName*in)
    {
        nc::net::PacketBuffer out;
        out.size = nc::net::WriteBuffer(out.data, 0u, static_cast<uint32_t>(in->packetType), in->name.c_str());
        return out;
    }
    
    nc::net::PacketBuffer ToPacketBuffer(const Packet::SpawnPrefab*in)
    {
        nc::net::PacketBuffer out;
        out.size = nc::net::WriteBuffer
        (
            out.data, 0u,
            static_cast<uint32_t>(in->eventType),
            static_cast<uint32_t>(in->resource),
            static_cast<uint32_t>(in->networkHandle),
            in->posX, in->posY, in->posZ,
            in->rotX, in->rotY, in->rotZ
        );
        return out;
    }

    nc::net::PacketBuffer ToPacketBuffer(const Packet::DestroyPrefab*in)
    {
        nc::net::PacketBuffer out;
        out.size = nc::net::WriteBuffer(out.data, 0u, static_cast<uint32_t>(in->eventType), static_cast<uint32_t>(in->networkHandle));
        return out;
    }

    nc::net::PacketBuffer ToPacketBuffer(const Packet::TestNetworkDispatcher* in)
    {
        nc::net::PacketBuffer out;
        out.size = nc::net::WriteBuffer(out.data, 0u, static_cast<uint32_t>(in->packetType), in->networkHandle, in->value);
        return out;
    }

    void FromByteArray(const uint8_t* in, Packet::ClientSendName* out)
    {
        nc::net::ReadBuffer(in, 0u, &out->name);
    }

    void FromByteArray(const uint8_t* in, Packet::SpawnPrefab* out)
    {
        nc::net::ReadBuffer
        (
            in, 0u,
            reinterpret_cast<uint32_t*>(&out->resource),
            reinterpret_cast<uint32_t*>(&out->networkHandle),
            &out->posX, &out->posY, &out->posZ,
            &out->rotX, &out->rotY, &out->rotZ
        );
    }

    void FromByteArray(const uint8_t* in, Packet::DestroyPrefab* out)
    {
        nc::net::ReadBuffer(in, 0u, &out->networkHandle);
    }

    void FromByteArray(const uint8_t* in, Packet::TestNetworkDispatcher* out)
    {
        nc::net::ReadBuffer(in, 0u, &out->networkHandle, &out->value);
    }
}