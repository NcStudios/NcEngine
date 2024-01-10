#include "gtest/gtest.h"
#include "graphics/GraphicsUtilities.h"

using namespace nc::graphics;

TEST(GraphicsUtilities_tests, AdjustDimensionsToAspectRatio_heightIsZero_doesntCrash)
{
    auto dimensions = nc::Vector2{500.0, 0.0f};
    ASSERT_NO_THROW(AdjustDimensionsToAspectRatio(dimensions));
}

TEST(GraphicsUtilities_tests, AdjustDimensionsToAspectRatio_widthLargerThanHeight_correctAspectRatio)
{
    auto dimensions = nc::Vector2{2000, 1000};
    auto adjustedDimensions = AdjustDimensionsToAspectRatio(dimensions);
    ASSERT_FLOAT_EQ(adjustedDimensions.x, 1777.7778f);
    ASSERT_FLOAT_EQ(adjustedDimensions.y, 1000.0f);
}

TEST(GraphicsUtilities_tests, AdjustDimensionsToAspectRatio_heightLargerThanWidth_correctAspectRatio)
{
    auto dimensions = nc::Vector2{1000, 2000};
    auto adjustedDimensions = AdjustDimensionsToAspectRatio(dimensions);
    ASSERT_FLOAT_EQ(adjustedDimensions.x, 1000.0f);
    ASSERT_FLOAT_EQ(adjustedDimensions.y, 562.5f);
}

TEST(GraphicsUtilities_tests, AdjustDimensionsToAspectRatio_alreadyInAspectRatio_noChange)
{
    auto dimensions = nc::Vector2{1600, 900};
    auto adjustedDimensions = AdjustDimensionsToAspectRatio(dimensions);
    ASSERT_FLOAT_EQ(adjustedDimensions.x, 1600.0f);
    ASSERT_FLOAT_EQ(adjustedDimensions.y, 900.0f);
}
