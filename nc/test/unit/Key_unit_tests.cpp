#include "gtest/gtest.h"

#include "utility/Key.h"
#include <cstdint>

using namespace nc;


TEST(Key_unit_tests, Key_SameValue)
{
    constexpr Key t0 = "Transform";

    std::string t1 = "Transform";
    Key t2 = t1;

    EXPECT_EQ(t0.value, t2.value);
}

TEST(Key_unit_tests, Key_Equal)
{
    constexpr Key t0 = "Transform";
    Key t1 = std::string{"Transform"};

    EXPECT_EQ(t0 == t1, true);
}

TEST(Key_unit_tests, Key_NotEqual)
{
    constexpr Key t0 = "Transform";
    Key t1 = std::string{"transform"};

    EXPECT_EQ(t0 != t1, true);
}

TEST(Key_unit_tests, Key_MapKey)
{
    std::map<Key, int> tags{};

    tags["Transform"] = 23;

    Key t0 = std::string{"Transform"};

    EXPECT_EQ(tags[t0], 23);
}

TEST(Key_unit_tests, KeyList_ConstExpr)
{
    constexpr KeyList keys = {"Transform", "Position"};

    EXPECT_EQ(keys.Size(), 2);
    EXPECT_EQ(keys.keys[0].text, "Transform");
    EXPECT_EQ(keys.keys[1].text, "Position");
    EXPECT_EQ(keys.keys[2].text, nullptr);
}

TEST(Key_unit_tests, KeyList_Equals)
{
    constexpr KeyList a = {"Transform", "Position"};

    std::string key0 = "Transform";
    std::string key1 = "Position";
    KeyList b = {key0, key1};

    EXPECT_EQ(a == b, true);
}

TEST(Key_unit_tests, KeyList_NotEquals)
{
    constexpr KeyList a = {"Transform", "Position"};

    std::string key0 = "transform";
    std::string key1 = "Position";
    KeyList b = {key0, key1};

    EXPECT_EQ(a == b, false);
}


int main(int argc, char ** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}