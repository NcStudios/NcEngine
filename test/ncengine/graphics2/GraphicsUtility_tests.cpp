#include "gtest/gtest.h"
#include "ncengine/graphics/GraphicsUtility.h"

#include <algorithm>

TEST(GraphicsUtilityTest, AdjustDimensionsToAspectRatio_heightIsZero_doesntCrash)
{
    auto dimensions = nc::Vector2{500.0, 0.0f};
    ASSERT_NO_THROW(nc::AdjustDimensionsToAspectRatio(dimensions));
}

TEST(GraphicsUtilityTest, AdjustDimensionsToAspectRatio_widthLargerThanHeight_correctAspectRatio)
{
    auto dimensions = nc::Vector2{2000, 1000};
    auto adjustedDimensions = nc::AdjustDimensionsToAspectRatio(dimensions);
    ASSERT_FLOAT_EQ(adjustedDimensions.x, 1777.7778f);
    ASSERT_FLOAT_EQ(adjustedDimensions.y, 1000.0f);
}

TEST(GraphicsUtilityTest, AdjustDimensionsToAspectRatio_heightLargerThanWidth_correctAspectRatio)
{
    auto dimensions = nc::Vector2{1000, 2000};
    auto adjustedDimensions = nc::AdjustDimensionsToAspectRatio(dimensions);
    ASSERT_FLOAT_EQ(adjustedDimensions.x, 1000.0f);
    ASSERT_FLOAT_EQ(adjustedDimensions.y, 562.5f);
}

TEST(GraphicsUtilityTest, AdjustDimensionsToAspectRatio_alreadyInAspectRatio_noChange)
{
    auto dimensions = nc::Vector2{1600, 900};
    auto adjustedDimensions = nc::AdjustDimensionsToAspectRatio(dimensions);
    ASSERT_FLOAT_EQ(adjustedDimensions.x, 1600.0f);
    ASSERT_FLOAT_EQ(adjustedDimensions.y, 900.0f);
}

TEST(GraphicsUtilityTest, GetPostProcessPassName_succeedsForAllPasses)
{
    for (const auto passId : nc::GetPostProcessPassFlags())
    {
        EXPECT_NO_THROW(nc::GetPostProcessPassName(passId));
    }
}

TEST(GraphicsUtilityTest, GetPostProcessEffectPassFlags_succeedsForAllEffects)
{
    for (const auto effectId : nc::GetPostProcessEffectIds())
    {
        EXPECT_NO_THROW(nc::GetPostProcessEffectPassFlags(effectId));
    }
}

TEST(GraphicsUtilityTest, GetCombinedPostProcessEffectPassFlags_succeedsForAllEffects)
{
    for (const auto effectId : nc::GetPostProcessEffectIds())
    {
        EXPECT_NO_THROW(nc::GetCombinedPostProcessEffectPassFlags(effectId));
    }
}

TEST(GraphicsUtilityTest, GetPostProcessEffectPassFlags_matchesCombinedFlags)
{
    for (const auto effectId : nc::GetPostProcessEffectIds())
    {
        const auto expectedCombined = nc::GetCombinedPostProcessEffectPassFlags(effectId);
        const auto enumerated = nc::GetPostProcessEffectPassFlags(effectId);
        const auto actualCombined = std::ranges::fold_left(
            enumerated,
            0ull,
            [](const auto& lhs, const auto& rhs) {
                return lhs | rhs;
            }
        );

        EXPECT_EQ(expectedCombined, actualCombined);
    }
}

TEST(GraphicsUtilityTest, CurveTypeConversion_roundTrip_succeeds)
{
    for (const auto& name : nc::GetCurveTypeNames())
    {
        const auto curveType = nc::ToCurveType(name);
        const auto curveTypeName = nc::ToString(curveType);
        EXPECT_EQ(name, curveTypeName);
    }
}
