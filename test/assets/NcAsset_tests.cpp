#include "ncengine/asset/NcAsset.h"
#include "ncengine/config/Config.h"
#include "gtest/gtest.h"

#include <ranges>

const auto g_assetSettings = nc::config::AssetSettings
{
    .audioClipsPath         = NC_TEST_COLLATERAL_DIRECTORY,
    .concaveCollidersPath   = NC_TEST_COLLATERAL_DIRECTORY,
    .hullCollidersPath      = NC_TEST_COLLATERAL_DIRECTORY,
    .meshesPath             = NC_TEST_COLLATERAL_DIRECTORY,
    .shadersPath            = NC_TEST_COLLATERAL_DIRECTORY,
    .skeletalAnimationsPath = NC_TEST_COLLATERAL_DIRECTORY,
    .texturesPath           = NC_TEST_COLLATERAL_DIRECTORY,
    .cubeMapsPath           = NC_TEST_COLLATERAL_DIRECTORY
};

const auto g_memorySettings = nc::config::MemorySettings{};

const auto g_defaultAssets = nc::asset::AssetMap
{
    {nc::asset::AssetType::AudioClip,         {"sound1.nca"}},
    {nc::asset::AssetType::ConcaveCollider,   {"concave_collider1.nca"}},
    {nc::asset::AssetType::CubeMap,           {"skybox1.nca"}},
    {nc::asset::AssetType::HullCollider,      {"hull_collider1.nca"}},
    {nc::asset::AssetType::Mesh,              {"mesh1.nca"}},
    {nc::asset::AssetType::SkeletalAnimation, {"test_animation.nca"}},
    {nc::asset::AssetType::Texture,           {"texture_base.nca", "texture_normal.nca"}}
};

TEST(NcAssetTests, GetLoadedAssets_returnsCompleteCollection)
{
    auto uut = nc::asset::BuildAssetModule(g_assetSettings, g_memorySettings, g_defaultAssets);
    uut->OnBeforeSceneLoad();
    const auto actualAssets = uut->GetLoadedAssets();
    EXPECT_EQ(g_defaultAssets.size(), actualAssets.size());
    for (const auto& [type, assets] : uut->GetLoadedAssets())
    {
        EXPECT_TRUE(std::ranges::equal(g_defaultAssets.at(type), assets));
    }
}
