#include "gtest/gtest.h"
#include "network/PacketBuffer.h"

#include <stdint.h>
#include <cstring>

using namespace nc::net;

namespace
{
    bool vBool = true;
    uint8_t vUint8 = 5u;
    uint32_t vUint32 = 500u;
    float vFloat = 2.5f;
    const char* vCharArray = "test";
}

TEST(PacketBuffer_unit_tests, Append_Bool_AdjustsSize)
{
    PacketBuffer buffer;
    buffer.size = static_cast<uint32_t>(WriteBuffer(buffer.data, 0u, vBool));
    EXPECT_EQ(buffer.size, sizeof(vBool));
}

TEST(PacketBuffer_unit_tests, Append_Uint8_AdjustsSize)
{
    PacketBuffer buffer;
    buffer.size = static_cast<uint32_t>(WriteBuffer(buffer.data, 0u, vUint8));
    EXPECT_EQ(buffer.size, sizeof(vUint8));
}

TEST(PacketBuffer_unit_tests, Append_Uint32_AdjustsSize)
{
    PacketBuffer buffer;
    buffer.size = static_cast<uint32_t>(WriteBuffer(buffer.data, 0u, vUint32));
    EXPECT_EQ(buffer.size, sizeof(vUint32));
}

TEST(PacketBuffer_unit_tests, Append_Float_AdjustsSize)
{
    PacketBuffer buffer;
    buffer.size = static_cast<uint32_t>(WriteBuffer(buffer.data, 0u, vFloat));
    EXPECT_EQ(buffer.size, sizeof(vFloat));
}

TEST(PacketBuffer_unit_tests, Append_String_AdjustsSize)
{
    PacketBuffer buffer;
    buffer.size = static_cast<uint32_t>(WriteBuffer(buffer.data, 0u, vCharArray));
    EXPECT_EQ(buffer.size, std::strlen(vCharArray) + 1u);
}

TEST(PacketBuffer_unit_tests, ReadBuffer_HeterogeneousData_OutputMatchesInput)
{
    PacketBuffer buffer;
    buffer.size = static_cast<uint32_t>(WriteBuffer(buffer.data, 0u, vFloat, vUint32, vCharArray, vUint8, vBool));

    float actualFloat;
    uint32_t actualUint32;
    std::string actualString;
    uint8_t actualUint8;
    bool actualBool;

    ReadBuffer(buffer.data, 0u, &actualFloat, &actualUint32, &actualString, &actualUint8, &actualBool);
    EXPECT_EQ(actualFloat, vFloat);
    EXPECT_EQ(actualUint32, vUint32);
    EXPECT_STREQ(actualString.c_str(), vCharArray);
    EXPECT_EQ(actualUint8, vUint8);
    EXPECT_EQ(actualBool, vBool);
}

int main(int argc, char ** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}