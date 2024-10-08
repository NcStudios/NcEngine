#include "gtest/gtest.h"
#include "graphics2/DiligentEngine.h"

using namespace nc::graphics;

TEST(DiligentEngine_tests, AdjustDimensionsToAspectRatio_heightIsZero_doesntCrash)
{
    auto dimensions = nc::Vector2{500.0, 0.0f};
    ASSERT_NO_THROW(AdjustDimensionsToAspectRatio(dimensions));
}
