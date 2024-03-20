#include "gtest/gtest.h"
#include "assets/manager/AssetUtilities.h"

#include "ncutility/Algorithm.h"

struct ProjectedType
{
    std::string str;
    int v;
};

TEST(AssetUtilitiesTest, HasValidAssetExtension_valid_returnsTrue)
{
    EXPECT_TRUE(nc::asset::HasValidAssetExtension("path/to a/valid.nca"));
    EXPECT_TRUE(nc::asset::HasValidAssetExtension("_.nca"));
    EXPECT_TRUE(nc::asset::HasValidAssetExtension("nca.nca"));
}

TEST(AssetUtilitiesTest, HasValidAssetExtension_invalid_returnsFalse)
{
    EXPECT_FALSE(nc::asset::HasValidAssetExtension(""));
    EXPECT_FALSE(nc::asset::HasValidAssetExtension("bad/path/to/nca"));
    EXPECT_FALSE(nc::asset::HasValidAssetExtension("nca."));
    EXPECT_FALSE(nc::asset::HasValidAssetExtension("nca"));
    EXPECT_FALSE(nc::asset::HasValidAssetExtension("another.txt"));
}

TEST(AssetUtilitiesTest, GetPaths_map_transformsCollection)
{
    auto map = std::unordered_map<std::string, float>{};
    EXPECT_EQ(0, nc::asset::GetPaths(map).size());
    map.emplace("a test key", 1.0f);
    map.emplace("another key", 2.0f);
    map.emplace("yet another", 3.0f);
    const auto actual = nc::asset::GetPaths(map);
    for (auto path : actual)
    {
        ASSERT_TRUE(map.contains(std::string{path.data()}));
    }
}

TEST(AssetUtilitiesTest, GetPaths_vectorOfString_transformsCollection)
{
    auto vec = std::vector<std::string>{};
    EXPECT_EQ(0, nc::asset::GetPaths(vec).size());
    vec.push_back("first");
    vec.push_back("second");
    vec.push_back("third");
    const auto actual = nc::asset::GetPaths(vec);
    EXPECT_TRUE(std::ranges::equal(vec, actual));
}

TEST(AssetUtilitiesTest, GetPaths_vectorProjection_transformsCollection)
{
    auto proj = [](const ProjectedType& obj) { return std::string_view{obj.str}; };
    auto vec = std::vector<ProjectedType>{};
    EXPECT_EQ(0, nc::asset::GetPaths(vec, proj).size());
    vec.emplace_back("first", 8);
    vec.emplace_back("second", 42);
    const auto actual = nc::asset::GetPaths(vec, proj);
    for (const auto& [i, path] : nc::algo::Enumerate(actual))
    {
        const auto& expected = vec.at(i).str;
        EXPECT_EQ(expected, path);
    }
}
