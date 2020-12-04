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

namespace project::network::Packet
{
    struct ClientSendName
    {
        const static nc::net::PacketType packetType = nc::net::PacketType::ClientSendName;
        std::string name;
    };

    struct SpawnPrefab
    {
        const static nc::net::PacketType eventType = nc::net::PacketType::SpawnPrefab;
        prefab::Resource resource;
        nc::net::NetworkHandle networkHandle;
        float posX, posY, posZ, rotX, rotY, rotZ;
    };

    struct DestroyPrefab
    {
        const static nc::net::PacketType eventType = nc::net::PacketType::DestroyPrefab;
        nc::net::NetworkHandle networkHandle;
    };

    struct TestNetworkDispatcher
    {
        const static nc::net::PacketType packetType = nc::net::PacketType::TestNetworkDispatcher;
        uint32_t networkHandle;
        float value;
    };
} // end namespace project::network::Packet

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

    nc::net::PacketBuffer ToPacketBuffer(const Packet::ClientSendName* in);
    nc::net::PacketBuffer ToPacketBuffer(const Packet::SpawnPrefab* in);
    nc::net::PacketBuffer ToPacketBuffer(const Packet::DestroyPrefab* in);
    nc::net::PacketBuffer ToPacketBuffer(const Packet::TestNetworkDispatcher* in);

    void FromByteArray(const uint8_t* in, Packet::ClientSendName* out);
    void FromByteArray(const uint8_t* in, Packet::SpawnPrefab* out);
    void FromByteArray(const uint8_t* in, Packet::DestroyPrefab* out);
    void FromByteArray(const uint8_t* in, Packet::TestNetworkDispatcher* out);
} // end namespace project::network