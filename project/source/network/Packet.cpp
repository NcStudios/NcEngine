#include "Packet.h"

#include <cstring>

namespace project::network
{
    PacketClientSendName::PacketClientSendName(std::string v1)
        : name{v1}
    {
    }

    PacketClientSendName::PacketClientSendName(uint8_t* data)
    {
        nc::net::ReadBuffer(data, 0u, &name);
    }

    nc::net::PacketBuffer PacketClientSendName::ToPacketBuffer()
    {
        nc::net::PacketBuffer out;
        out.size = nc::net::WriteBuffer(out.data, 0u, static_cast<uint32_t>(packetType), name.c_str());
        return out;
    }

    PacketSpawnPrefab::PacketSpawnPrefab(prefab::Resource res, nc::net::NetworkHandle handle, float pX, float pY, float pZ, float rX, float rY, float rZ)
        : resource{res},
          networkHandle{handle},
          posX{pX}, posY{pY}, posZ{pZ},
          rotX{rX}, rotY{rY}, rotZ{rZ}
    {
    }

    PacketSpawnPrefab::PacketSpawnPrefab(uint8_t* data)
    {
        nc::net::ReadBuffer
        (
            data, 0u,
            reinterpret_cast<uint32_t*>(&resource),
            reinterpret_cast<uint32_t*>(&networkHandle),
            &posX, &posY, &posZ,
            &rotX, &rotY, &rotZ
        );
    }

    nc::net::PacketBuffer PacketSpawnPrefab::ToPacketBuffer()
    {
        nc::net::PacketBuffer out;
        out.size = nc::net::WriteBuffer
        (
            out.data, 0u,
            static_cast<uint32_t>(eventType),
            static_cast<uint32_t>(resource),
            static_cast<uint32_t>(networkHandle),
            posX, posY, posZ,
            rotX, rotY, rotZ
        );
        return out;
    }


    PacketDestroyPrefab::PacketDestroyPrefab(nc::net::NetworkHandle handle)
        : networkHandle{handle}
    {
    }

    PacketDestroyPrefab::PacketDestroyPrefab(uint8_t* data)
    {
        nc::net::ReadBuffer(data, 0u, &networkHandle);
    }

    nc::net::PacketBuffer PacketDestroyPrefab::ToPacketBuffer()
    {
        nc::net::PacketBuffer out;
        out.size = nc::net::WriteBuffer(out.data, 0u, static_cast<uint32_t>(eventType), static_cast<uint32_t>(networkHandle));
        return out;
    }

    PacketTestNetworkDispatcher::PacketTestNetworkDispatcher(uint32_t v1, float v2)
        : networkHandle{ v1 },
          value{ v2 }
    {
    }

    PacketTestNetworkDispatcher::PacketTestNetworkDispatcher(uint8_t* data)
        : networkHandle{ 0u },
          value{ 0.0f }
    {
        nc::net::ReadBuffer(data, 0u, &networkHandle, &value);
    }

    nc::net::PacketBuffer PacketTestNetworkDispatcher::ToPacketBuffer()
    {
        nc::net::PacketBuffer out;
        out.size = nc::net::WriteBuffer(out.data, 0u, static_cast<uint32_t>(packetType), networkHandle, value);
        return out;
    }
}