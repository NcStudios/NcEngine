#include "gtest/gtest.h"
#include "ncutility/Compression.h"

#include <algorithm>
#include <array>

constexpr auto g_data = std::array<char, 32>{
    0x1, 0x2, 0x3, 0x4, 0x1, 0x2, 0x3, 0x4,
    0x1, 0x2, 0x3, 0x4, 0x1, 0x2, 0x3, 0x4,
    0x1, 0x2, 0x3, 0x4, 0x1, 0x2, 0x3, 0x4,
    0x1, 0x2, 0x3, 0x4, 0x1, 0x2, 0x3, 0x4
};

TEST(CompressionTest, RoundTrip_defaultCompression_preservesData)
{
    const auto compressed = nc::Compress(g_data, nc::CompressionLevel::Default);
    const auto actual = nc::Decompress(compressed, g_data.size());
    EXPECT_TRUE(std::ranges::equal(g_data, actual));
}

TEST(CompressionTest, RoundTrip_fastCompression_preservesData)
{
    const auto compressed = nc::Compress(g_data, nc::CompressionLevel::Fast);
    const auto actual = nc::Decompress(compressed, g_data.size());
    EXPECT_TRUE(std::ranges::equal(g_data, actual));
}

TEST(CompressionTest, RoundTrip_maxCompression_preservesData)
{
    const auto compressed = nc::Compress(g_data, nc::CompressionLevel::Max);
    const auto actual = nc::Decompress(compressed, g_data.size());
    EXPECT_TRUE(std::ranges::equal(g_data, actual));
}

TEST(CompressionTest, RoundTrip_emptyData_preservesData)
{
    const auto expected = std::vector<char>{};
    const auto compressed = nc::Compress(expected);
    const auto actual = nc::Decompress(compressed, 100);
    EXPECT_TRUE(std::ranges::equal(expected, actual));
}

TEST(CompressionTest, RoundTrip_uncompressableData_preservesData)
{
    constexpr auto expected = std::array<char, 1>{'a'};
    const auto compressed = nc::Compress(expected, nc::CompressionLevel::Default);
    const auto actual = nc::Decompress(compressed, expected.size());
    EXPECT_TRUE(std::ranges::equal(expected, actual));
}

TEST(CompressionTest, Compress_invalidCompressionLevel_throws)
{
    constexpr auto badLevel = static_cast<nc::CompressionLevel>(100);
    EXPECT_THROW(nc::Compress(g_data, badLevel), std::exception);
}

TEST(CompressionTest, Decompress_excessiveSizeParam_shrinksToActualSize)
{
    const auto compressed = nc::Compress(g_data, nc::CompressionLevel::Default);
    const auto actual = nc::Decompress(compressed, g_data.size() * 2ull);
    ASSERT_EQ(g_data.size(), actual.size());
    EXPECT_TRUE(std::ranges::equal(g_data, actual));
}

TEST(CompressionTest, Decompress_insufficientSizeParam_throws)
{
    const auto compressed = nc::Compress(g_data, nc::CompressionLevel::Default);
    EXPECT_THROW(nc::Decompress(compressed, 10), std::exception);
}
