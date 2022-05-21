#include "gtest/gtest.h"
#include "math/Math.h"

TEST(Math_unit_tests, Min_FirstSmaller_ReturnsFirst)
{
    auto smaller = 0u;
    auto larger = 3u;
    EXPECT_EQ(nc::math::Min(smaller, larger), smaller);
}

TEST(Math_unit_tests, Min_SecondSmaller_ReturnsSecond)
{
    auto smaller = 0.0f;
    auto larger = 3.0f;
    EXPECT_EQ(nc::math::Min(larger, smaller), smaller);
}

TEST(Math_unit_tests, Max_FirstLarger_ReturnsFirst)
{
    auto smaller = 0u;
    auto larger = 3u;
    EXPECT_EQ(nc::math::Max(larger, smaller), larger);
}

TEST(Math_unit_tests, Max_SecondLarger_ReturnsSecond)
{
    auto smaller = 0.0f;
    auto larger = 3.0f;
    EXPECT_EQ(nc::math::Max(smaller, larger), larger);
}

TEST(Math_unit_tests, Clamp_ValueLessThanMin_ReturnsMin)
{
    auto value = 1.7f;
    auto min = 2.0f;
    auto max = 3.0f;
    auto actual = nc::math::Clamp(value, min, max);
    EXPECT_EQ(actual, min);
}

TEST(Math_unit_tests, Clamp_ValueGreaterThanMax_ReturnsMax)
{
    auto value = 2.7f;
    auto min = 1.0f;
    auto max = 2.0f;
    auto actual = nc::math::Clamp(value, min, max);
    EXPECT_EQ(actual, max);
}

TEST(Math_unit_tests, Clamp_ValueBetweenMinMax_ReturnsSameValue)
{
    auto value = 1.5f;
    auto min = 1.0f;
    auto max = 2.0f;
    auto actual = nc::math::Clamp(value, min, max);
    EXPECT_EQ(actual, value);
}

TEST(Math_unit_tests, Lerp_FactorZero_ReturnsFrom)
{
    auto from = 1.0f;
    auto to = 2.0f;
    auto factor = 0.0f;
    auto actual = nc::math::Lerp(from, to, factor);
    EXPECT_EQ(actual, from);
}

TEST(Math_unit_tests, Lerp_FactorOne_ReturnsTo)
{
    auto from = 1.0f;
    auto to = 10.0f;
    auto factor = 1.0f;
    auto actual = nc::math::Lerp(from, to, factor);
    EXPECT_EQ(actual, to);
}

TEST(Math_unit_tests, Lerp_FactorGreaterThanOne_ClampsFactorToOne)
{
    auto from = 1.0f;
    auto to = 10.0f;
    auto factor = 5.0f;
    auto actual = nc::math::Lerp(from, to, factor);
    EXPECT_EQ(actual, to);
}

TEST(Math_unit_tests, Lerp_FactorOneHalf_ReturnsMidPoint)
{
    auto from = 6.0f;
    auto to = 8.0f;
    auto factor = 0.5f;
    auto actual = nc::math::Lerp(from, to, factor);
    EXPECT_EQ(actual, 7.0f);
}

TEST(Math_unit_tests, WrapAngle_InputLessThanTwoPi_ReturnsInput)
{
    auto angle = std::numbers::pi;
    auto actual = nc::math::WrapAngle(angle);
    EXPECT_EQ(actual, angle);
}

TEST(Math_unit_tests, WrapAngle_InputEqualsTwoPi_ReturnsZero)
{
    auto angle = 2.0 * std::numbers::pi;
    auto actual = nc::math::WrapAngle(angle);
    auto expected = 0.0;
    EXPECT_EQ(actual, expected);
}

TEST(Math_unit_tests, WrapAngle_InputGreaterThanTwoPi_ReturnsWrappedAngle)
{
    auto angle = 3.0 * std::numbers::pi;
    auto actual = nc::math::WrapAngle(angle);
    auto expected = std::numbers::pi;
    EXPECT_EQ(actual, expected);
}

TEST(Math_unit_tests, DegreesToRadians_ConvertsUnits)
{
    auto degrees = 180.0;
    auto actual = nc::math::DegreesToRadians(degrees);
    EXPECT_EQ(actual, std::numbers::pi);
}

TEST(Math_unit_tests, RadiansToDegrees_ConvertsUnits) 
{
    auto radians = std::numbers::pi;
    auto actual = nc::math::RadiansToDegrees(radians);
    EXPECT_EQ(actual, 180.0);
}

int main(int argc, char ** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}