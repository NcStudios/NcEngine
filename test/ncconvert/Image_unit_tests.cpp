#include "gtest/gtest.h"
#include "utility/Image.h"

#include "CollateralTexture.h"
#include "TextureTestUtility.h"

#include "ncasset/Assets.h"
#include "ncasset/AssetType.h"
#include "ncutility/NcError.h"

#include <algorithm>
#include <array>
#include <cstring>

TEST(ImageTest, Constructor_validPath_readsFile)
{
    namespace test_data = collateral::rgb_corners;

    {
        const auto uut = nc::convert::Image{test_data::bmpFilePath};
        EXPECT_EQ(test_data::width, uut.GetWidth());
        EXPECT_EQ(test_data::height, uut.GetHeight());
        EXPECT_EQ(test_data::numBytes, uut.GetSizeInBytes());
    }

    {
        const auto uut = nc::convert::Image{test_data::jpgFilePath};
        EXPECT_EQ(test_data::width, uut.GetWidth());
        EXPECT_EQ(test_data::height, uut.GetHeight());
        EXPECT_EQ(test_data::numBytes, uut.GetSizeInBytes());
    }

    {
        const auto uut = nc::convert::Image{test_data::pngFilePath};
        EXPECT_EQ(test_data::width, uut.GetWidth());
        EXPECT_EQ(test_data::height, uut.GetHeight());
        EXPECT_EQ(test_data::numBytes, uut.GetSizeInBytes());
    }
}

TEST(ImageTest, Constructor_badPath_throws)
{
    EXPECT_THROW(nc::convert::Image{"not/a/path.jpg"}, nc::NcError);
}

TEST(ImageTest, MakeTextureSubResource_convertsPixels)
{
    namespace test_data = collateral::rgb_corners;
    const auto uut = nc::convert::Image{test_data::pngFilePath};
    const auto actual = uut.MakeTextureSubResource();
    EXPECT_EQ(test_data::width, actual.width);
    EXPECT_EQ(test_data::height, actual.height);
    ASSERT_EQ(test_data::numBytes, actual.pixelData.size());

    for (auto pixelIndex = 0u; pixelIndex < test_data::numPixels; ++pixelIndex)
    {
        const auto expectedPixel = test_data::pixels[pixelIndex];
        const auto actualPixel = ReadPixel(actual.pixelData.data(), pixelIndex * 4);
        EXPECT_EQ(expectedPixel, actualPixel);
    }
}

TEST(ImageTest, Compress_allFormats_compressesPixels)
{
    namespace test_data = collateral::rgb_corners;
    const auto uut = nc::convert::Image{test_data::pngFilePath};

    using enum nc::asset::TextureFormat;
    const auto actualBC1SRGB = uut.Compress(BC1_UNORM_SRGB);
    const auto actualBC1     = uut.Compress(BC1_UNORM);
    const auto actualBC3SRGB = uut.Compress(BC3_UNORM_SRGB);
    const auto actualBC3     = uut.Compress(BC3_UNORM);

    #define COMPARE_SUBRESOURCE(actual)                      \
    EXPECT_EQ(test_data::width, actual.width);               \
    EXPECT_EQ(test_data::height, actual.height);             \
    EXPECT_GT(test_data::numBytes, actual.pixelData.size()); \

    COMPARE_SUBRESOURCE(actualBC1SRGB);
    COMPARE_SUBRESOURCE(actualBC1);
    COMPARE_SUBRESOURCE(actualBC3SRGB);
    COMPARE_SUBRESOURCE(actualBC3);

    #undef COMPARE_SUBRESOURCE
}

TEST(ImageTest, Compress_invalidFormat_throws)
{
    namespace test_data = collateral::rgb_corners;
    const auto uut = nc::convert::Image{test_data::pngFilePath};
    EXPECT_THROW(uut.Compress(nc::asset::TextureFormat::RGBA8_UNORM_SRGB), nc::NcError);
    EXPECT_THROW(uut.Compress(nc::asset::TextureFormat::RGBA8_UNORM), nc::NcError);
}

TEST(ImageTest, Resize_downsample_scalesImage)
{
    namespace test_data = collateral::rgb_corners;
    auto uut = nc::convert::Image{test_data::pngFilePath};
    const auto expectedWidth = test_data::width / 2;
    const auto expectedHeight = test_data::height / 2;
    const auto expectedSizeInBytes = expectedWidth * expectedHeight * 4;
    uut.Resize(expectedWidth, expectedHeight);

    EXPECT_EQ(expectedWidth, uut.GetWidth());
    EXPECT_EQ(expectedHeight, uut.GetHeight());
    EXPECT_EQ(expectedSizeInBytes, uut.GetSizeInBytes());
}

TEST(ImageTest, Resize_upsample_scalesImage)
{
    namespace test_data = collateral::rgb_corners;
    auto uut = nc::convert::Image{test_data::pngFilePath};
    const auto expectedWidth = test_data::width * 4;
    const auto expectedHeight = test_data::height * 4;
    const auto expectedSizeInBytes = expectedWidth * expectedHeight * 4;
    uut.Resize(expectedWidth, expectedHeight);

    EXPECT_EQ(expectedWidth, uut.GetWidth());
    EXPECT_EQ(expectedHeight, uut.GetHeight());
    EXPECT_EQ(expectedSizeInBytes, uut.GetSizeInBytes());
}

constexpr unsigned char g_inputRGBA[4] = {100, 150, 200, 250};
constexpr auto g_inputPixel =
    (g_inputRGBA[0] << 24) |
    (g_inputRGBA[1] << 16) |
    (g_inputRGBA[2] << 8)  |
    (g_inputRGBA[3]);

constexpr auto g_paddingRGBA = nc::convert::Image::paddingRGBA;
constexpr auto g_paddingPixel =
    (g_paddingRGBA[0] << 24) |
    (g_paddingRGBA[1] << 16) |
    (g_paddingRGBA[2] << 8)  |
    (g_paddingRGBA[3]);


struct PaddingCheckResult
{
    bool equal = false;
    int x = 0;
    int y = 0;
    uint32_t expected = 0;
    uint32_t actual = 0;

    operator bool() const { return equal; }
};

auto operator<< (std::ostream& stream, const PaddingCheckResult& result) -> std::ostream&
{
    stream << "expected: " << result.expected
           << "\nactual: " << result.actual
           << "\nx: "      << result.x
           << "\ny: "      << result.y;

    return stream;
}

// verify a padded image has exactly the same pixels in the top left corner as the input
auto CheckPaddedImage(const nc::convert::Image& image,
                      int originalWidth,
                      int originalHeight) -> PaddingCheckResult
{
    const auto data = image.GetData();
    const auto width = image.GetWidth();
    const auto height = image.GetHeight();
    for (auto x = 0; x < width; ++x)
    {
        for (auto y = 0; y < height; ++y)
        {
            const auto pos = x * 4 + y * width * 4;
            const auto actualPixel = ReadPixel(data, pos);
            if (x < originalWidth && y < originalHeight)
            {
                if (actualPixel != g_inputPixel)
                {
                    return PaddingCheckResult{false, x, y, g_inputPixel, actualPixel};
                }
            }
            else if (actualPixel != g_paddingPixel)
            {
                return PaddingCheckResult{false, x, y, g_paddingPixel, actualPixel};
            }
        }
    }

    return PaddingCheckResult{true};
}

TEST(ImageTest, ResizePadded_tooSmall_resizes)
{
    // 2x2 -> 4x4
    constexpr auto inputWidth = 2;
    constexpr auto inputHeight = 2;
    auto uut = nc::convert::Image{inputWidth, inputHeight, g_inputRGBA};
    uut.ResizePadded();

    EXPECT_EQ(4, uut.GetWidth());
    EXPECT_EQ(4, uut.GetHeight());
    EXPECT_EQ(16 * 4, uut.GetSizeInBytes());
    const auto result = CheckPaddedImage(uut, inputWidth, inputHeight);
    EXPECT_TRUE(result) << result;
}

TEST(ImageTest, ResizePadded_onDimensionTooSmall_resizesOneDimension)
{
    // 1x8 -> 4x8
    constexpr auto inputWidth = 1;
    constexpr auto inputHeight = 8;
    auto uut = nc::convert::Image{inputWidth, inputHeight, g_inputRGBA};
    uut.ResizePadded();

    EXPECT_EQ(4, uut.GetWidth());
    EXPECT_EQ(8, uut.GetHeight());
    EXPECT_EQ(4 * 8 * 4, uut.GetSizeInBytes());
    const auto result = CheckPaddedImage(uut, inputWidth, inputHeight);
    EXPECT_TRUE(result) << result;
}

TEST(ImageTest, ResizePadded_invalidDimensions_resizes)
{
    // 13x5 -> 16x8
    constexpr auto inputWidth = 13;
    constexpr auto inputHeight = 5;
    auto uut = nc::convert::Image{inputWidth, inputHeight, g_inputRGBA};
    uut.ResizePadded();

    EXPECT_EQ(16, uut.GetWidth());
    EXPECT_EQ(8, uut.GetHeight());
    EXPECT_EQ(16 * 8 * 4, uut.GetSizeInBytes());
    const auto result = CheckPaddedImage(uut, inputWidth, inputHeight);
    EXPECT_TRUE(result) << result;
}

TEST(ImageTest, ResizePadded_oneInvalidDimension_resizesOneDimension)
{
    // 15x8 -> 16x8
    constexpr auto inputWidth = 15;
    constexpr auto inputHeight = 8;
    auto uut = nc::convert::Image{inputWidth, inputHeight, g_inputRGBA};
    uut.ResizePadded();

    EXPECT_EQ(16, uut.GetWidth());
    EXPECT_EQ(8, uut.GetHeight());
    EXPECT_EQ(8 * 16 * 4, uut.GetSizeInBytes());
    const auto result = CheckPaddedImage(uut, inputWidth, inputHeight);
    EXPECT_TRUE(result) << result;
}

TEST(ImageTest, ResizePadded_dimensionsAlreadyValid_doesNotModify)
{
    // 32x4 -> unchanged
    constexpr auto inputWidth = 32;
    constexpr auto inputHeight = 4;
    auto uut = nc::convert::Image{inputWidth, inputHeight, g_inputRGBA};
    uut.ResizePadded();

    EXPECT_EQ(32, uut.GetWidth());
    EXPECT_EQ(4, uut.GetHeight());
    EXPECT_EQ(32 * 4 * 4, uut.GetSizeInBytes());
    const auto result = CheckPaddedImage(uut, inputWidth, inputHeight);
    EXPECT_TRUE(result) << result;
}
