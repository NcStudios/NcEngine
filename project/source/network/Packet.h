#pragma once

#include "nc/source/net/PacketBuffer.h"

#include <stdint.h>
#include <string>


namespace project::network
{
    enum class PacketType : uint32_t
    {
        Test,
        ServerRequestClientName,
        ClientSendName
    };

    struct PacketTest
    {
        PacketTest(uint32_t v1, float v2, const char* v3, bool v4);
        PacketTest(uint8_t* data, size_t size);
        nc::net::PacketBuffer ToPacketBuffer();

        const static PacketType eventType = PacketType::Test;
        uint32_t num;
        float fp;
        std::string str;
        bool boolean;

        void Print();
    };

    struct PacketClientSendName
    {
        PacketClientSendName(std::string v1);
        PacketClientSendName(uint8_t* data, size_t size);
        nc::net::PacketBuffer ToPacketBuffer();

        const static PacketType packetType = PacketType::ClientSendName;
        std::string name;

        void Print();
    };
}