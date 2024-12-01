#include "gtest/gtest.h"
#include "ncengine/graphics/GraphicsUtility.h"

#include <algorithm>

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
