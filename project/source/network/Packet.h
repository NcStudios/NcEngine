#pragma once

#include "nc/source/net/NetworkDetails.h"
#include "nc/source/net/PacketBuffer.h"

#include <stdint.h>
#include <string>
#include <utility>

/**
 * General Packet Formats
 * 
 * Forwarded to NetworkedPrefab:
 *     Bytes  Value
 *     -----  -----
 *     0-3    PacketType
 *     4-7    NetworkHandle
 *     8-n    Struct Members
 * 
 * Handled directly by EventCoordinator:
 *     Bytes  Value
 *     -----  -----
 *     0-3    PacketType
 *     4-n    Struct Members
 */

namespace project::prefab { enum class Resource : uint32_t; }

namespace nc::net
{
    enum class PacketType : uint32_t
    {
        ClientSendName,
        SpawnPrefab,
        DestroyPrefab,
        TestNetworkDispatcher
    };
}

namespace project::network
{
    inline nc::net::PacketType GetPacketTypeFromBuffer(uint8_t* data)
    {
        return *reinterpret_cast<nc::net::PacketType*>(data);
    }

    inline nc::net::NetworkHandle GetNetworkHandleFromBuffer(uint8_t* data)
    {
        return *reinterpret_cast<nc::net::NetworkHandle*>(data + nc::net::PacketTypeSize);
    }

    inline std::pair<nc::net::PacketType, nc::net::NetworkHandle> ReadPacketHeader(uint8_t* data)
    {
        return { *reinterpret_cast<nc::net::PacketType*>(data),
                 *reinterpret_cast<nc::net::NetworkHandle*>(data + nc::net::PacketTypeSize) };
    }

    struct PacketClientSendName
    {
        PacketClientSendName(std::string v1);
        PacketClientSendName(uint8_t* data);
        nc::net::PacketBuffer ToPacketBuffer();

        const static nc::net::PacketType packetType = nc::net::PacketType::ClientSendName;
        std::string name;
    };

    struct PacketSpawnPrefab
    {
        PacketSpawnPrefab(prefab::Resource resource, nc::net::NetworkHandle handle, float posX, float posY, float poxZ, float rotX, float rotY, float rotZ);
        PacketSpawnPrefab(uint8_t* data);
        nc::net::PacketBuffer ToPacketBuffer();

        const static nc::net::PacketType eventType = nc::net::PacketType::SpawnPrefab;
        prefab::Resource resource;
        nc::net::NetworkHandle networkHandle;
        float posX, posY, posZ, rotX, rotY, rotZ;
    };

    struct PacketDestroyPrefab
    {
        PacketDestroyPrefab(nc::net::NetworkHandle handle);
        PacketDestroyPrefab(uint8_t* data);
        nc::net::PacketBuffer ToPacketBuffer();

        const static nc::net::PacketType eventType = nc::net::PacketType::DestroyPrefab;
        nc::net::NetworkHandle networkHandle;
    };

    struct PacketTestNetworkDispatcher
    {
        PacketTestNetworkDispatcher(uint32_t networkHandle, float value);
        PacketTestNetworkDispatcher(uint8_t* data);

        nc::net::PacketBuffer ToPacketBuffer();
        const static nc::net::PacketType packetType = nc::net::PacketType::TestNetworkDispatcher;
        uint32_t networkHandle;
        float value;
    };
}