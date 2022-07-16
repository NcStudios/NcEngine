#include "gtest/gtest.h"
#include "asset/AssetId.h"

#include <fstream>
#include <set>
#include <string>
#include <vector>

using namespace nc;

constexpr auto path1 = "mesh/myMesh.nca";
constexpr auto path2 = "mesh/myMesh2.nca";

TEST(AssetId_tests, Constructor_CompileTime_Succeeds)
{
    constexpr auto uut = AssetId<AssetType::Mesh>(path1);
    constexpr auto actual = uut.Hash();
    constexpr auto expected = hash::Fnv1a(path1);
    EXPECT_EQ(actual, expected);
    /** Prove compile time execution with a static_assert */
    static_assert(actual == expected);
}

TEST(AssetId_tests, Constructor_Runtime_Succeeds)
{
    const auto path = std::string{path1};
    const auto id = AssetId<AssetType::Mesh>(path);
    const auto actual = id.Hash();
    const auto expected = hash::Fnv1a(path);
    EXPECT_EQ(actual, expected);
}

TEST(AssetId_tests, EmptyString_ReturnsFnvBasis)
{
    constexpr auto uut = AssetId<AssetType::Mesh>("");
    constexpr auto actual = uut.Hash();
    constexpr auto expected = hash::FnvOffsetBasis;
    EXPECT_EQ(actual, expected);
}

TEST(AssetId_tests, ComparisonOperators_SameAssets_ReturnEqual)
{
    constexpr auto id1 = AssetId<AssetType::Mesh>(path1);
    constexpr auto id2 = AssetId<AssetType::Mesh>(path1);
    EXPECT_TRUE(id1 == id2);
    EXPECT_FALSE(id1 != id2);
}

TEST(AssetId_tests, ComparisonOperators_DifferentAssets_ReturnsNotEqual)
{
    constexpr auto id1 = AssetId<AssetType::Mesh>(path1);
    constexpr auto id2 = AssetId<AssetType::Mesh>(path2);
    EXPECT_FALSE(id1 == id2);
    EXPECT_TRUE(id1 != id2);
}

auto ReadCollateral() -> std::vector<std::string>
{
    std::vector<std::string> out;
    std::ifstream inFile{NC_TEST_COLLATERAL_DIRECTORY"word_list.txt"};
    std::string path;
    while(inFile)
    {
        inFile >> path;
        out.push_back(std::move(path));
    }
    return out;
}

TEST(AssetId_tests, Fnv1a_HashMany_NoCollisions)
{
    /** @note With enough input we will get collisions. This is just checking
     *  nothing is horribly wrong with the implementation. */
    const auto paths = ReadCollateral();
    auto set = std::set<size_t>{};
    for(const auto& path : paths)
    {
        set.emplace(AssetId<AssetType::Mesh>{path}.Hash());
    }
    EXPECT_EQ(set.size(), paths.size());
}

int main(int argc, char ** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}