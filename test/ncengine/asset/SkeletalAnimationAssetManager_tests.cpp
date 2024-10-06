#include "gtest/gtest.h"
#include "asset/AssetData.h"
#include "asset/manager/SkeletalAnimationAssetManager.h"

#include <array>
#include <string>

using namespace nc::asset;

const auto test_animation = "test_animation.nca";
const auto test_animation_2 = "test_animation_2.nca";

class SkeletalAnimationAssetManager_tests : public ::testing::Test
{
    public:
        std::unique_ptr<SkeletalAnimationAssetManager> assetManager;

    protected:
        void SetUp() override
        {
            assetManager = std::make_unique<SkeletalAnimationAssetManager>(NC_TEST_COLLATERAL_DIRECTORY, 1000);
        }

        void TearDown() override
        {
            assetManager = nullptr;
        }
};

TEST_F(SkeletalAnimationAssetManager_tests, Load_NotLoaded_ReturnsTrue)
{
    auto actual = assetManager->Load(test_animation, false);
    EXPECT_TRUE(actual);
}

TEST_F(SkeletalAnimationAssetManager_tests, Load_IsLoaded_ReturnsFalse)
{
    assetManager->Load(test_animation, false);
    auto actual = assetManager->Load(test_animation, false);
    EXPECT_FALSE(actual);
}

TEST_F(SkeletalAnimationAssetManager_tests, Load_BadPath_Throws)
{
    EXPECT_THROW(assetManager->Load("bad/path", false), std::runtime_error);
}

TEST_F(SkeletalAnimationAssetManager_tests, Load_Collection_ReturnsTrue)
{
    std::array<std::string, 2u> paths {test_animation, test_animation_2};
    auto actual = assetManager->Load(paths, false);
    EXPECT_TRUE(actual);
}

TEST_F(SkeletalAnimationAssetManager_tests, Load_Collection_OneAlreadyLoaded_IndexRemainsTheSame)
{
    std::array<std::string, 2u> paths {test_animation, test_animation_2};
    assetManager->Load(paths, false);
    auto viewFirstLoad = assetManager->Acquire(test_animation);

    std::array<std::string, 1u> paths2 {test_animation};
    assetManager->Load(paths2, false);

    auto viewSecondLoad = assetManager->Acquire(test_animation);
    EXPECT_EQ(viewFirstLoad.index, 0u);
    EXPECT_EQ(viewSecondLoad.index, 0u);
}

TEST_F(SkeletalAnimationAssetManager_tests, Unload_Loaded_ReturnsTrue)
{
    assetManager->Load(test_animation, false);
    auto actual = assetManager->Unload(test_animation);
    EXPECT_TRUE(actual);
}

TEST_F(SkeletalAnimationAssetManager_tests, Unload_NotLoaded_ReturnsFalse)
{
    auto actual = assetManager->Unload(test_animation);
    EXPECT_FALSE(actual);
}

TEST_F(SkeletalAnimationAssetManager_tests, IsLoaded_Loaded_ReturnsTrue)
{
    assetManager->Load(test_animation, false);
    auto actual = assetManager->IsLoaded(test_animation);
    EXPECT_TRUE(actual);
}

TEST_F(SkeletalAnimationAssetManager_tests, IsLoaded_NotLoaded_ReturnsFalse)
{
    auto actual = assetManager->IsLoaded(test_animation);
    EXPECT_FALSE(actual);
}

TEST_F(SkeletalAnimationAssetManager_tests, UnloadAll_HasAssets_RemovesAssets)
{
    std::array<std::string, 2u> paths {test_animation, test_animation_2};
    assetManager->Load(paths, false);
    assetManager->UnloadAll(AssetFlags::None);
    EXPECT_FALSE(assetManager->IsLoaded(test_animation));
    EXPECT_FALSE(assetManager->IsLoaded(test_animation));
}

TEST_F(SkeletalAnimationAssetManager_tests, UnloadAll_Empty_Completes)
{
    assetManager->UnloadAll(AssetFlags::None);
}

TEST_F(SkeletalAnimationAssetManager_tests, GetPath_Loaded_ReturnsPath)
{
    std::array<std::string, 2u> paths{test_animation, test_animation_2};
    assetManager->Load(paths, false);
    const auto& expected = paths.at(0);
    const auto view = assetManager->Acquire(expected);
    const auto actual = assetManager->GetPath(view.id);
    EXPECT_EQ(expected, actual);
}

TEST_F(SkeletalAnimationAssetManager_tests, GetPath_NotLoaded_Throws)
{
    std::array<std::string, 2u> paths{test_animation, test_animation_2};
    assetManager->Load(paths, false);
    const auto& expected = paths.at(0);
    const auto view = assetManager->Acquire(expected);
    assetManager->UnloadAll();
    EXPECT_THROW(assetManager->GetPath(view.id), nc::NcError);
}
