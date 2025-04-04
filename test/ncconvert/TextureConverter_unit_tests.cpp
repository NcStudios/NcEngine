#include "gtest/gtest.h"
#include "CollateralTexture.h"
#include "TextureTestUtility.h"

#include "converters/TextureConverter.h"
#include "analysis/TextureAnalysis.h"
#include "utility/EnumExtensions.h"
#include "ncasset/Assets.h"

#include <algorithm>
#include <array>
#include <cstring>
#include <ranges>

TEST(TextureConverterTest, ImportTexture_png_convertsToNca)
{
    namespace test_data = collateral::rgb_corners;
    auto uut = nc::convert::TextureConverter{};
    constexpr auto expectedFormat = nc::asset::TextureFormat::RGBA8_UNORM_SRGB;
    const auto actual = uut.ImportTexture(test_data::pngFilePath, expectedFormat, false);

    EXPECT_EQ(expectedFormat, actual.format);
    EXPECT_EQ(test_data::width, actual.width);
    EXPECT_EQ(test_data::height, actual.height);
    ASSERT_EQ(test_data::numBytes, actual.pixelData.size());
    EXPECT_TRUE(actual.mipmaps.empty());

    for (auto pixelIndex = 0u; pixelIndex < test_data::numPixels; ++pixelIndex)
    {
        const auto expectedPixel = test_data::pixels[pixelIndex];
        const auto actualPixel = ReadPixel(actual.pixelData.data(), pixelIndex * 4);
        EXPECT_EQ(expectedPixel, actualPixel);
    }
}

TEST(TextureConverterTest, ImportTexture_jpg_convertsToNca)
{
    namespace test_data = collateral::rgb_corners;
    auto uut = nc::convert::TextureConverter{};
    constexpr auto expectedFormat = nc::asset::TextureFormat::RGBA8_UNORM;
    const auto actual = uut.ImportTexture(test_data::jpgFilePath, expectedFormat, false);

    EXPECT_EQ(expectedFormat, actual.format);
    EXPECT_EQ(test_data::width, actual.width);
    EXPECT_EQ(test_data::height, actual.height);
    ASSERT_EQ(test_data::numBytes, actual.pixelData.size());
    EXPECT_TRUE(actual.mipmaps.empty());

    // Not testing pixels because jpeg
}

TEST(TextureConverterTest, ImportTexture_bmp_convertsToNca)
{
    namespace test_data = collateral::rgb_corners;
    auto uut = nc::convert::TextureConverter{};
    constexpr auto expectedFormat = nc::asset::TextureFormat::RGBA8_UNORM_SRGB;
    const auto actual = uut.ImportTexture(test_data::bmpFilePath, nc::asset::TextureFormat::RGBA8_UNORM_SRGB, false);

    EXPECT_EQ(expectedFormat, actual.format);
    EXPECT_EQ(test_data::width, actual.width);
    EXPECT_EQ(test_data::height, actual.height);
    ASSERT_EQ(test_data::numBytes, actual.pixelData.size());
    EXPECT_TRUE(actual.mipmaps.empty());

    for (auto pixelIndex = 0u; pixelIndex < test_data::numPixels; ++pixelIndex)
    {
        const auto expectedPixel = test_data::pixels[pixelIndex];
        const auto actualPixel = ReadPixel(actual.pixelData.data(), pixelIndex * 4);
        EXPECT_EQ(expectedPixel, actualPixel);
    }
}

TEST(TextureConverterTest, ImportTexture_bc1Compression_convertsToNca)
{
    namespace test_data = collateral::rgb_corners;
    auto uut = nc::convert::TextureConverter{};
    constexpr auto expectedFormat = nc::asset::TextureFormat::BC1_UNORM_SRGB;
    const auto actual = uut.ImportTexture(test_data::pngFilePath, expectedFormat, true);

    EXPECT_EQ(expectedFormat, actual.format);
    EXPECT_EQ(test_data::width, actual.width);
    EXPECT_EQ(test_data::height, actual.height);
    ASSERT_EQ(test_data::numBytes, actual.pixelData.size() * 8); // BC1 gives 8:1 compression

    const auto minDimension = nc::convert::GetMinimumDimension(expectedFormat);
    const auto mipLevels = nc::convert::GetMipLevels(actual.width, actual.height, minDimension);
    ASSERT_GT(mipLevels, 1); // ensure collateral supports mip chain > 1
    const auto expectedMipmaps = mipLevels - 1;
    ASSERT_EQ(expectedMipmaps, actual.mipmaps.size());

    auto halfWidth = std::max(actual.width / 2, minDimension);
    auto halfHeight = std::max(actual.height / 2, minDimension);
    for (const auto& mipmap : actual.mipmaps)
    {
        EXPECT_EQ(halfWidth, mipmap.width);
        EXPECT_EQ(halfHeight, mipmap.height);
        const auto uncompressedSize = halfWidth * halfHeight * nc::asset::Texture::numChannels;
        EXPECT_GT(uncompressedSize, mipmap.pixelData.size());
        halfWidth = std::max(halfWidth / 2, minDimension);
        halfHeight = std::max(halfHeight / 2, minDimension);
    }
}

TEST(TextureConverterTest, ImportTexture_bc3Compression_convertsToNca)
{
    namespace test_data = collateral::rgb_corners;
    auto uut = nc::convert::TextureConverter{};
    constexpr auto expectedFormat = nc::asset::TextureFormat::BC3_UNORM_SRGB;
    const auto actual = uut.ImportTexture(test_data::pngFilePath, expectedFormat, true);

    EXPECT_EQ(expectedFormat, actual.format);
    EXPECT_EQ(test_data::width, actual.width);
    EXPECT_EQ(test_data::height, actual.height);
    ASSERT_EQ(test_data::numBytes, actual.pixelData.size() * 4); // BC3 gives 4:1 compression

    const auto minDimension = nc::convert::GetMinimumDimension(expectedFormat);
    const auto mipLevels = nc::convert::GetMipLevels(actual.width, actual.height, minDimension);
    ASSERT_GT(mipLevels, 1); // ensure collateral supports mip chain > 1
    const auto expectedMipmaps = mipLevels - 1;
    ASSERT_EQ(expectedMipmaps, actual.mipmaps.size());

    auto halfWidth = std::max(actual.width / 2, minDimension);
    auto halfHeight = std::max(actual.height / 2, minDimension);
    for (const auto& mipmap : actual.mipmaps)
    {
        EXPECT_EQ(halfWidth, mipmap.width);
        EXPECT_EQ(halfHeight, mipmap.height);
        const auto uncompressedSize = halfWidth * halfHeight * nc::asset::Texture::numChannels;
        EXPECT_GT(uncompressedSize, mipmap.pixelData.size());
        halfWidth = std::max(halfWidth / 2, minDimension);
        halfHeight = std::max(halfHeight / 2, minDimension);
    }
}

TEST(TextureConverterTest, ImportTexture_requestMips_generatesMips)
{
    namespace test_data = collateral::rgb_corners;
    auto uut = nc::convert::TextureConverter{};
    constexpr auto expectedFormat = nc::asset::TextureFormat::RGBA8_UNORM;
    const auto actual = uut.ImportTexture(test_data::pngFilePath, expectedFormat, true);

    EXPECT_EQ(expectedFormat, actual.format);
    EXPECT_EQ(test_data::width, actual.width);
    EXPECT_EQ(test_data::height, actual.height);
    ASSERT_EQ(test_data::numBytes, actual.pixelData.size());
    for (auto pixelIndex = 0u; pixelIndex < test_data::numPixels; ++pixelIndex)
    {
        const auto expectedPixel = test_data::pixels[pixelIndex];
        const auto actualPixel = ReadPixel(actual.pixelData.data(), pixelIndex * 4);
        EXPECT_EQ(expectedPixel, actualPixel);
    }

    const auto minDimension = nc::convert::GetMinimumDimension(expectedFormat);
    const auto mipLevels = nc::convert::GetMipLevels(actual.width, actual.height, minDimension);
    ASSERT_GT(mipLevels, 1); // ensure collateral supports mip chain > 1
    const auto expectedMipmaps = mipLevels - 1;
    ASSERT_EQ(expectedMipmaps, actual.mipmaps.size());

    auto halfWidth = std::max(actual.width / 2, minDimension);
    auto halfHeight = std::max(actual.height / 2, minDimension);
    for (const auto& mipmap : actual.mipmaps)
    {
        EXPECT_EQ(halfWidth, mipmap.width);
        EXPECT_EQ(halfHeight, mipmap.height);
        const auto expectedSize = halfWidth * halfHeight * nc::asset::Texture::numChannels;
        EXPECT_EQ(expectedSize, mipmap.pixelData.size());
        halfWidth = std::max(halfWidth / 2, minDimension);
        halfHeight = std::max(halfHeight / 2, minDimension);
    }
}

TEST(TextureConverterTest, ImportCubeMap_horizontalArray_convertsToNca)
{
    namespace test_data = collateral::cube_map;
    auto uut = nc::convert::TextureConverter{};
    const auto format = nc::asset::TextureFormat::RGBA8_UNORM_SRGB;
    const auto actual = uut.ImportCubeMap(test_data::horizontalArrayFilePath, format);

    EXPECT_EQ(format, actual.format);
    EXPECT_EQ(test_data::faceSideLength, actual.faceSideLength);

    for (const auto [expectedFace, actualFace] : std::views::zip(test_data::faces, actual.faces))
    {
        ASSERT_EQ(expectedFace.size() * 4, actualFace.size());
        for (auto pixelIndex = 0u; pixelIndex < expectedFace.size(); ++pixelIndex)
        {
            const auto expectedPixel = expectedFace.at(pixelIndex);
            const auto actualPixel = ReadPixel(actualFace.data(), pixelIndex * 4);
            EXPECT_EQ(expectedPixel, actualPixel);
        }
    }
}

TEST(TextureConverterTest, ImportCubeMap_verticalArray_convertsToNca)
{
    namespace test_data = collateral::cube_map;
    auto uut = nc::convert::TextureConverter{};
    const auto format = nc::asset::TextureFormat::RGBA8_UNORM_SRGB;
    const auto actual = uut.ImportCubeMap(test_data::verticalArrayFilePath, format);

    EXPECT_EQ(format, actual.format);
    EXPECT_EQ(test_data::faceSideLength, actual.faceSideLength);

    for (const auto [expectedFace, actualFace] : std::views::zip(test_data::faces, actual.faces))
    {
        ASSERT_EQ(expectedFace.size() * 4, actualFace.size());
        for (auto pixelIndex = 0u; pixelIndex < expectedFace.size(); ++pixelIndex)
        {
            const auto expectedPixel = expectedFace.at(pixelIndex);
            const auto actualPixel = ReadPixel(actualFace.data(), pixelIndex * 4);
            EXPECT_EQ(expectedPixel, actualPixel);
        }
    }
}

TEST(TextureConverterTest, ImportCubeMap_horizontalCross_convertsToNca)
{
    namespace test_data = collateral::cube_map;
    auto uut = nc::convert::TextureConverter{};
    const auto format = nc::asset::TextureFormat::RGBA8_UNORM_SRGB;
    const auto actual = uut.ImportCubeMap(test_data::horizontalCrossFilePath, format);

    EXPECT_EQ(format, actual.format);
    EXPECT_EQ(test_data::faceSideLength, actual.faceSideLength);

    for (const auto [expectedFace, actualFace] : std::views::zip(test_data::faces, actual.faces))
    {
        ASSERT_EQ(expectedFace.size() * 4, actualFace.size());
        for (auto pixelIndex = 0u; pixelIndex < expectedFace.size(); ++pixelIndex)
        {
            const auto expectedPixel = expectedFace.at(pixelIndex);
            const auto actualPixel = ReadPixel(actualFace.data(), pixelIndex * 4);
            EXPECT_EQ(expectedPixel, actualPixel);
        }
    }
}

TEST(TextureConverterTest, ImportCubeMap_verticalCross_convertsToNca)
{
    namespace test_data = collateral::cube_map;
    auto uut = nc::convert::TextureConverter{};
    const auto format = nc::asset::TextureFormat::RGBA8_UNORM_SRGB;
    const auto actual = uut.ImportCubeMap(test_data::verticalCrossFilePath, format);

    EXPECT_EQ(format, actual.format);
    EXPECT_EQ(test_data::faceSideLength, actual.faceSideLength);

    for (const auto [expectedFace, actualFace] : std::views::zip(test_data::faces, actual.faces))
    {
        ASSERT_EQ(expectedFace.size() * 4, actualFace.size());
        for (auto pixelIndex = 0u; pixelIndex < expectedFace.size(); ++pixelIndex)
        {
            const auto expectedPixel = expectedFace.at(pixelIndex);
            const auto actualPixel = ReadPixel(actualFace.data(), pixelIndex * 4);
            EXPECT_EQ(expectedPixel, actualPixel);
        }
    }
}
