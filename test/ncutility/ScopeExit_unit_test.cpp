#include "gtest/gtest.h"
#include "ncutility/ScopeExit.h"

TEST(ScopeExitTests, SingleExpression_callsOnExit)
{
    const auto expected = 5;
    auto actual = 4;

    {
        SCOPE_EXIT(actual = expected;);
    }

    EXPECT_EQ(expected, actual);
}

TEST(ScopeExitTests, MultiExpression_callsAllOnExit)
{
    const auto expected1 = 5;
    const auto expected2 = 2.0f;
    const auto expected3 = "newString";
    auto actual1 = 4;
    auto actual2 = 1.0f;
    auto actual3 = "oldString";

    {
        SCOPE_EXIT(
            actual1 = expected1;
            actual2 = expected2;
            actual3 = expected3;
        );
    }

    EXPECT_EQ(expected1, actual1);
    EXPECT_EQ(expected2, actual2);
    EXPECT_STREQ(expected3, actual3);
}

TEST(ScopeExitTests, MultipleObjectsInSameScope_callsAllOnExit)
{
    const auto expected1 = 5;
    const auto expected2 = 6;
    auto actual1 = 1;
    auto actual2 = 2;

    {
        SCOPE_EXIT(actual1 = expected1;);
        SCOPE_EXIT(actual2 = expected2;);
    }

    EXPECT_EQ(expected1, actual1);
    EXPECT_EQ(expected2, actual2);
}
