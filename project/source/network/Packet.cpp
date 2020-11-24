#include "Packet.h"

#include <cstring>
#include <iostream>

namespace project::network
{
    PacketTest::PacketTest(uint32_t v1, float v2, const char* v3, bool v4)
        : num{ v1 },
          fp{ v2 },
          str{ v3 },
          boolean{ v4 }
    {
    }

    PacketTest::PacketTest(uint8_t* data, size_t size)
        : num{ 0u },
          fp{ 0.0f },
          str{ },
          boolean{ false }
    {
        (void)size;
        nc::net::ReadBuffer(data, 0u, &num, &fp, &str, &boolean);
    }

    nc::net::PacketBuffer PacketTest::ToPacketBuffer()
    {
        nc::net::PacketBuffer out;
        out.size = nc::net::WriteBuffer(out.data, 0u, static_cast<uint32_t>(eventType), num, fp, str.c_str(), boolean);
        return out;
    }

    void PacketTest::Print()
    {
        std::cout << "PacketTest:\n"
                  << "  num:  " << num << '\n'
                  << "  fp:   " << fp  << '\n'
                  << "  str:  " << str << '\n'
                  << "  bool: " << boolean << '\n';
    }

    PacketClientSendName::PacketClientSendName(std::string v1)
        : name{v1}
    {
    }

    PacketClientSendName::PacketClientSendName(uint8_t* data, size_t size)
    {
        (void)size;
        nc::net::ReadBuffer(data, 0u, &name);
    }

    nc::net::PacketBuffer PacketClientSendName::ToPacketBuffer()
    {
        nc::net::PacketBuffer out;
        out.size = nc::net::WriteBuffer(out.data, 0u, static_cast<uint32_t>(packetType), name.c_str());
        return out;
    }

    void PacketClientSendName::Print()
    {
        std::cout << "PacketClientSendName\n"
                  << "  name: " << name << '\n';
    }

}