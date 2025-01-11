#include "gtest/gtest.h"
#include "ncjolt/ByteArrayStream.h"

#include <array>

TEST(ByteArrayStreamTest, DefaultConstructor)
{
    auto uut = nc::jolt::ByteArrayStream{};

    EXPECT_FALSE(uut.IsFailed());
    EXPECT_FALSE(uut.IsEOF());
}

TEST(ByteArrayStreamTest, ConstructorWithVector)
{
    auto uut = nc::jolt::ByteArrayStream{ {1, 2, 3, 4, 5} };

    const auto& actualBuffer = uut.GetBuffer();
    EXPECT_FALSE(uut.IsFailed());
    EXPECT_FALSE(uut.IsEOF());
    EXPECT_EQ(actualBuffer[0], 1);
    EXPECT_EQ(actualBuffer[1], 2);
    EXPECT_EQ(actualBuffer[2], 3);
    EXPECT_EQ(actualBuffer[3], 4);
    EXPECT_EQ(actualBuffer[4], 5);
}

TEST(ByteArrayStreamTest, ReadBytes_goodCall_copiesData)
{
    auto uut = nc::jolt::ByteArrayStream{ {10, 20, 30} };
    auto buffer = std::array<uint8_t, 3>{};
    uut.ReadBytes(buffer.data(), 3);

    EXPECT_EQ(buffer[0], 10);
    EXPECT_EQ(buffer[1], 20);
    EXPECT_EQ(buffer[2], 30);
    EXPECT_FALSE(uut.IsEOF());
    EXPECT_FALSE(uut.IsFailed());
}

TEST(ByteArrayStreamTest, ReadBytes_pastEnd_copiesFirstDataAndSetsEOF)
{
    auto uut = nc::jolt::ByteArrayStream{ {10, 20} };
    auto buffer = std::array<uint8_t, 4>{};
    uut.ReadBytes(buffer.data(), 4);

    EXPECT_TRUE(uut.IsFailed());
    EXPECT_TRUE(uut.IsEOF());
    EXPECT_EQ(buffer[0], 10);
    EXPECT_EQ(buffer[1], 20);
    EXPECT_EQ(buffer[2], 0); // remaining bytes should not be set
    EXPECT_EQ(buffer[3], 0);
}

TEST(ByteArrayStreamTest, ReadBytes_emptyStream_setsFailAndEOF)
{
    auto uut = nc::jolt::ByteArrayStream{};
    auto buffer = std::array<uint8_t, 1>{};
    uut.ReadBytes(buffer.data(), 1);

    EXPECT_TRUE(uut.IsFailed());
    EXPECT_TRUE(uut.IsEOF());
}

TEST(ByteArrayStreamTest, WriteBytes_goodCall_copiesData)
{
    auto uut = nc::jolt::ByteArrayStream{};
    constexpr auto expected = std::array<uint8_t, 3>{100, 200, 255};
    uut.WriteBytes(expected.data(), expected.size());

    const auto& actual = uut.GetBuffer();
    ASSERT_EQ(expected.size(), actual.size());
    EXPECT_EQ(expected[0], actual[0]);
    EXPECT_EQ(expected[1], actual[1]);
    EXPECT_EQ(expected[2], actual[2]);
}

TEST(ByteArrayStreamTest, WriteAndRead_roundTrip_preservesData)
{
    auto uut = nc::jolt::ByteArrayStream{};
    constexpr auto expected = std::array<uint8_t, 4>{5, 10, 15, 20};
    uut.WriteBytes(expected.data(), 4);
    auto actual = std::array<uint8_t, 4>{};
    uut.ReadBytes(actual.data(), 4);

    EXPECT_EQ(expected[0], actual[0]);
    EXPECT_EQ(expected[1], actual[1]);
    EXPECT_EQ(expected[2], actual[2]);
    EXPECT_EQ(expected[3], actual[3]);
    EXPECT_FALSE(uut.IsEOF());
    EXPECT_FALSE(uut.IsFailed());
}

TEST(ByteArrayStreamTest, ExtractBuffer_hasExpectedData)
{
    const auto expected = std::vector<uint8_t>{10, 20, 30};
    auto uut = nc::jolt::ByteArrayStream{expected};
    const auto actual = uut.ExtractBuffer();

    EXPECT_EQ(expected.size(), actual.size());
    EXPECT_EQ(expected[0], actual[0]);
    EXPECT_EQ(expected[1], actual[1]);
    EXPECT_EQ(expected[2], actual[2]);
    EXPECT_FALSE(uut.IsFailed());
    EXPECT_FALSE(uut.IsEOF());
    EXPECT_EQ(uut.GetBuffer().size(), 0);
}

TEST(ByteArrayStreamTest, EOFBehavior)
{
    auto uut = nc::jolt::ByteArrayStream{ {42} };
    auto buffer = std::array<uint8_t, 1>{};
    uut.ReadBytes(buffer.data(), 1);

    EXPECT_EQ(buffer[0], 42);
    EXPECT_FALSE(uut.IsEOF());
    EXPECT_FALSE(uut.IsFailed());

    uut.ReadBytes(buffer.data(), 1);
    EXPECT_TRUE(uut.IsFailed());
}
