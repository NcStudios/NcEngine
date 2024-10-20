#include "gtest/gtest.h"
#include "asset/AssetId.h"

#include <string>

using namespace nc;

constexpr auto path1 = "mesh/myMesh.nca";
constexpr auto path2 = "mesh/myMesh2.nca";

TEST(AssetId_tests, Constructor_CompileTime_Succeeds)
{
    constexpr auto uut = AssetId<asset::AssetType::Mesh>(path1);
    constexpr auto actual = uut.Id();
    constexpr auto expected = utility::StringHash(path1).Hash();
    EXPECT_EQ(actual, expected);
    /** Prove compile time execution with a static_assert */
    static_assert(actual == expected);
}

TEST(AssetId_tests, Constructor_Runtime_Succeeds)
{
    const auto path = std::string{path1};
    const auto id = AssetId<asset::AssetType::Mesh>(path);
    const auto actual = id.Id();
    const auto expected = utility::StringHash(path).Hash();
    EXPECT_EQ(actual, expected);
}

TEST(AssetId_tests, Comparison_Same_ReturnEqual)
{
    constexpr auto id1 = AssetId<asset::AssetType::Mesh>(path1);
    constexpr auto id2 = AssetId<asset::AssetType::Mesh>(path1);
    EXPECT_TRUE(id1 == id2);
    EXPECT_FALSE(id1 != id2);
}

TEST(AssetId_tests, Comparison_Different_ReturnsNotEqual)
{
    constexpr auto id1 = AssetId<asset::AssetType::Mesh>(path1);
    constexpr auto id2 = AssetId<asset::AssetType::Mesh>(path2);
    EXPECT_FALSE(id1 == id2);
    EXPECT_TRUE(id1 != id2);
}
