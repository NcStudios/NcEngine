#include "gtest/gtest.h"

#include "animation/Ease.h"

using namespace nc;


TEST(Ease_unit_tests, Ease_Linear)
{
    EXPECT_EQ(Ease::Linear.Compute(0), 0);
    EXPECT_EQ(Ease::Linear.Compute(0.5), 0.5);
    EXPECT_EQ(Ease::Linear.Compute(1), 1);
}


TEST(Ease_unit_tests, Ease_Quad)
{
    EXPECT_EQ(Ease::Quad.Compute(0), 0);
    EXPECT_EQ(Ease::Quad.Compute(0.5), 0.5 * 0.5);
    EXPECT_EQ(Ease::Quad.Compute(1), 1);
}


int main(int argc, char ** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}