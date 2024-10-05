#include "gtest/gtest.h"
#include "analysis/TextureAnalysis.h"
#include "ncasset/Assets.h"

#include "ncutility/NcError.h"

namespace nc::convert
{
auto operator==(const SubTexturePos& lhs, const SubTexturePos& rhs) -> bool
{
    return lhs.x == rhs.x && lhs.y == rhs.y;
}
}

namespace
{
const auto horizontalArrayAtlas = nc::asset::Texture{
    .width = 6, .height = 1,
    .pixelData = std::vector<unsigned char>{
        0, 0, 0, 0, 0, 0
    }
};

const auto verticalArrayAtlas = nc::asset::Texture{
    .width = 1, .height = 6,
    .pixelData = std::vector<unsigned char>{
        0, 0, 0, 0, 0, 0
    }
};

const auto horizontalCrossAtlas = nc::asset::Texture{
    .width = 4, .height = 3,
    .pixelData = std::vector<unsigned char>{
        0, 0, 0, 0,   0, 0, 0, 0,   0, 0, 0, 0
    }
};

const auto verticalCrossAtlas = nc::asset::Texture{
    .width = 3, .height = 4,
    .pixelData = std::vector<unsigned char>{
        0, 0, 0,   0, 0, 0,   0, 0, 0,   0, 0, 0,
    }
};

const auto badAtlas1 = nc::asset::Texture{
    .width = 1, .height = 1,
    .pixelData = std::vector<unsigned char>{
        0
    }
};

const auto badAtlas2 = nc::asset::Texture{
    .width = 2, .height = 2,
    .pixelData = std::vector<unsigned char>{
        0, 0,   0, 0
    }
};

const auto badAtlas3 = nc::asset::Texture{
    .width = 3, .height = 5,
    .pixelData = std::vector<unsigned char>{
        0, 0, 0,   0, 0, 0,   0, 0, 0,   0, 0, 0,   0, 0, 0
    }
};

const auto badAtlas4 = nc::asset::Texture{
    .width = 5, .height = 2,
    .pixelData = std::vector<unsigned char>{
        0, 0, 0, 0, 0,   0, 0, 0, 0, 0
    }
};
}

TEST(TextureAnalysisTest, GetCubeMapAtlasLayout_horizontalArray)
{
    const auto actual = nc::convert::GetCubeMapAtlasLayout(horizontalArrayAtlas);
    EXPECT_EQ(nc::convert::CubeMapAtlasLayout::HorizontalArray, actual);
}

TEST(TextureAnalysisTest, GetCubeMapAtlasLayout_verticalArray)
{
    const auto actual = nc::convert::GetCubeMapAtlasLayout(verticalArrayAtlas);
    EXPECT_EQ(nc::convert::CubeMapAtlasLayout::VerticalArray, actual);
}

TEST(TextureAnalysisTest, GetCubeMapAtlasLayout_horizontalCross)
{
    const auto actual = nc::convert::GetCubeMapAtlasLayout(horizontalCrossAtlas);
    EXPECT_EQ(nc::convert::CubeMapAtlasLayout::HorizontalCross, actual);
}

TEST(TextureAnalysisTest, GetCubeMapAtlasLayout_verticalCross)
{
    const auto actual = nc::convert::GetCubeMapAtlasLayout(verticalCrossAtlas);
    EXPECT_EQ(nc::convert::CubeMapAtlasLayout::VerticalCross, actual);
}

TEST(TextureAnalysisTest, GetCubeMapAtlasLayout_invalidDimensions_throws)
{
    EXPECT_THROW(nc::convert::GetCubeMapAtlasLayout(badAtlas1), nc::NcError);
    EXPECT_THROW(nc::convert::GetCubeMapAtlasLayout(badAtlas2), nc::NcError);
    EXPECT_THROW(nc::convert::GetCubeMapAtlasLayout(badAtlas3), nc::NcError);
    EXPECT_THROW(nc::convert::GetCubeMapAtlasLayout(badAtlas4), nc::NcError);
}

TEST(TextureAnalysisTest, GetSubTextureInfo_horizontalArray)
{
    constexpr auto expectedSideLength = 1u;
    constexpr auto expectedFrontPos = nc::convert::SubTexturePos{4, 0};
    constexpr auto expectedBackPos = nc::convert::SubTexturePos{5, 0};
    constexpr auto expectedTopPos = nc::convert::SubTexturePos{2, 0};
    constexpr auto expectedBottomPos = nc::convert::SubTexturePos{3, 0};
    constexpr auto expectedRightPos = nc::convert::SubTexturePos{0, 0};
    constexpr auto expectedLeftPos = nc::convert::SubTexturePos{1, 0};

    const auto actual = nc::convert::GetSubTextureInfo(horizontalArrayAtlas);
    EXPECT_EQ(expectedSideLength, actual.sideLength);
    EXPECT_EQ(expectedFrontPos, actual.frontPosition);
    EXPECT_EQ(expectedBackPos, actual.backPosition);
    EXPECT_EQ(expectedTopPos, actual.topPosition);
    EXPECT_EQ(expectedBottomPos, actual.bottomPosition);
    EXPECT_EQ(expectedRightPos, actual.rightPosition);
    EXPECT_EQ(expectedLeftPos, actual.leftPosition);
}

TEST(TextureAnalysisTest, GetSubTextureInfo_verticalArray)
{
    constexpr auto expectedSideLength = 1u;
    constexpr auto expectedFrontPos = nc::convert::SubTexturePos{0, 4};
    constexpr auto expectedBackPos = nc::convert::SubTexturePos{0, 5};
    constexpr auto expectedTopPos = nc::convert::SubTexturePos{0, 2};
    constexpr auto expectedBottomPos = nc::convert::SubTexturePos{0, 3};
    constexpr auto expectedRightPos = nc::convert::SubTexturePos{0, 0};
    constexpr auto expectedLeftPos = nc::convert::SubTexturePos{0, 1};

    const auto actual = nc::convert::GetSubTextureInfo(verticalArrayAtlas);
    EXPECT_EQ(expectedSideLength, actual.sideLength);
    EXPECT_EQ(expectedFrontPos, actual.frontPosition);
    EXPECT_EQ(expectedBackPos, actual.backPosition);
    EXPECT_EQ(expectedTopPos, actual.topPosition);
    EXPECT_EQ(expectedBottomPos, actual.bottomPosition);
    EXPECT_EQ(expectedRightPos, actual.rightPosition);
    EXPECT_EQ(expectedLeftPos, actual.leftPosition);
}

TEST(TextureAnalysisTest, GetSubTextureInfo_horizontalCross)
{
    constexpr auto expectedSideLength = 1u;
    constexpr auto expectedFrontPos = nc::convert::SubTexturePos{1, 1};
    constexpr auto expectedBackPos = nc::convert::SubTexturePos{3, 1};
    constexpr auto expectedTopPos = nc::convert::SubTexturePos{1, 0};
    constexpr auto expectedBottomPos = nc::convert::SubTexturePos{1, 2};
    constexpr auto expectedRightPos = nc::convert::SubTexturePos{2, 1};
    constexpr auto expectedLeftPos = nc::convert::SubTexturePos{0, 1};

    const auto actual = nc::convert::GetSubTextureInfo(horizontalCrossAtlas);
    EXPECT_EQ(expectedSideLength, actual.sideLength);
    EXPECT_EQ(expectedFrontPos, actual.frontPosition);
    EXPECT_EQ(expectedBackPos, actual.backPosition);
    EXPECT_EQ(expectedTopPos, actual.topPosition);
    EXPECT_EQ(expectedBottomPos, actual.bottomPosition);
    EXPECT_EQ(expectedRightPos, actual.rightPosition);
    EXPECT_EQ(expectedLeftPos, actual.leftPosition);
}

TEST(TextureAnalysisTest, GetSubTextureInfo_verticalCross)
{
    constexpr auto expectedSideLength = 1u;
    constexpr auto expectedFrontPos = nc::convert::SubTexturePos{1, 1};
    constexpr auto expectedBackPos = nc::convert::SubTexturePos{1, 3};
    constexpr auto expectedTopPos = nc::convert::SubTexturePos{1, 0};
    constexpr auto expectedBottomPos = nc::convert::SubTexturePos{1, 2};
    constexpr auto expectedRightPos = nc::convert::SubTexturePos{2, 1};
    constexpr auto expectedLeftPos = nc::convert::SubTexturePos{0, 1};

    const auto actual = nc::convert::GetSubTextureInfo(verticalCrossAtlas);
    EXPECT_EQ(expectedSideLength, actual.sideLength);
    EXPECT_EQ(expectedFrontPos, actual.frontPosition);
    EXPECT_EQ(expectedBackPos, actual.backPosition);
    EXPECT_EQ(expectedTopPos, actual.topPosition);
    EXPECT_EQ(expectedBottomPos, actual.bottomPosition);
    EXPECT_EQ(expectedRightPos, actual.rightPosition);
    EXPECT_EQ(expectedLeftPos, actual.leftPosition);
}

TEST(TextureAnalysisTest, GetSubTextureInfo_invalidDimensions_throws)
{
    EXPECT_THROW(nc::convert::GetSubTextureInfo(badAtlas1), nc::NcError);
    EXPECT_THROW(nc::convert::GetSubTextureInfo(badAtlas2), nc::NcError);
    EXPECT_THROW(nc::convert::GetSubTextureInfo(badAtlas3), nc::NcError);
    EXPECT_THROW(nc::convert::GetSubTextureInfo(badAtlas4), nc::NcError);
}
