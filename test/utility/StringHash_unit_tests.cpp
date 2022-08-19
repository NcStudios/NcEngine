#include "gtest/gtest.h"
#include "utility/Hash.h"

#include <fstream>
#include <set>
#include <string>
#include <vector>

using namespace nc;

constexpr auto testString1 = "Some test input";
constexpr auto testString2 = "More input, but different";

auto ReadCollateral() -> std::vector<std::string>
{
    std::vector<std::string> out;
    std::ifstream inFile{NC_HASH_TEST_COLLATERAL_DIRECTORY"word_list.txt"};
    std::string path;
    while(inFile)
    {
        inFile >> path;
        out.push_back(std::move(path));
    }
    return out;
}

TEST(StringHash_unit_tests, Constructor_CompileTime_Succeeds)
{
    constexpr auto uut = utility::StringHash(testString1);
    constexpr auto actual = uut.Hash();
    constexpr auto expected = utility::Fnv1a(testString1);
    EXPECT_EQ(actual, expected);
    /** Prove compile time execution with a static_assert */
    static_assert(actual == expected);
}

TEST(StringHash_unit_tests, Constructor_Runtime_Succeeds)
{
    const auto path = std::string{testString1};
    const auto id = utility::StringHash(path);
    const auto actual = id.Hash();
    const auto expected = utility::Fnv1a(path);
    EXPECT_EQ(actual, expected);
}

TEST(StringHash_unit_tests, EmptyString_ReturnsFnvBasis)
{
    constexpr auto uut = utility::StringHash("");
    constexpr auto actual = uut.Hash();
    constexpr auto expected = utility::detail::FnvOffsetBasis;
    EXPECT_EQ(actual, expected);
}

TEST(StringHash_unit_tests, Comparison_Same_ReturnEqual)
{
    constexpr auto id1 = utility::StringHash(testString1);
    constexpr auto id2 = utility::StringHash(testString1);
    EXPECT_TRUE(id1 == id2);
    EXPECT_FALSE(id1 != id2);
}

TEST(StringHash_unit_tests, Comparison_Different_ReturnsNotEqual)
{
    constexpr auto id1 = utility::StringHash(testString1);
    constexpr auto id2 = utility::StringHash(testString2);
    EXPECT_FALSE(id1 == id2);
    EXPECT_TRUE(id1 != id2);
}

TEST(StringHash_unit_tests, Fnv1a_HashMany_NoCollisions)
{
    /** @note With enough input we will get collisions. This is just checking
     *  nothing is horribly wrong with the implementation. */
    const auto paths = ReadCollateral();
    auto set = std::set<size_t>{};
    for(const auto& path : paths)
    {
        set.emplace(utility::Fnv1a(path));
    }
    EXPECT_EQ(set.size(), paths.size());
}

int main(int argc, char ** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}