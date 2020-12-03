#pragma once

#include <cstdint>

namespace nc::net
{
    enum class PacketType : uint32_t;

    using NetworkHandle = uint32_t;
    const NetworkHandle NullNetworkHandle = 0u;

    constexpr auto PacketTypeSize = sizeof(PacketType);
    constexpr auto PacketHeaderSize = PacketTypeSize + sizeof(NetworkHandle);

    constexpr auto MaxClients = 7u;

    constexpr auto ChannelLimit = 2u;
    enum class Channel : uint8_t
    {
        Reliable = 0u,
        Unreliable = 1u
    };
} // end namespace nc::net