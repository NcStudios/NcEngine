#include "gtest/gtest.h"
#include "assets/manager/ConcaveColliderAssetManager.h"

#include "ncasset/Assets.h"

#include <array>
#include <string>

using namespace nc::asset;

const auto ConcavePath1 = "concave_collider1.nca";
const auto ConcavePath2 = "concave_collider2.nca";

class ConcaveColliderAssetManager_tests : public ::testing::Test
{
    public:
        std::unique_ptr<ConcaveColliderAssetManager> assetManager;

    protected:
        void SetUp() override
        {
            assetManager = std::make_unique<ConcaveColliderAssetManager>(NC_TEST_COLLATERAL_DIRECTORY);
        }

        void TearDown() override
        {
            assetManager = nullptr;
        }
};

TEST_F(ConcaveColliderAssetManager_tests, Load_NotLoaded_ReturnsTrue)
{
    auto actual = assetManager->Load(ConcavePath1, false);
    EXPECT_TRUE(actual);
}

TEST_F(ConcaveColliderAssetManager_tests, Load_Loaded_ReturnsFalse)
{
    assetManager->Load(ConcavePath1, false);
    auto actual = assetManager->Load(ConcavePath1, false);
    EXPECT_FALSE(actual);
}

TEST_F(ConcaveColliderAssetManager_tests, Load_BadPath_Throws)
{
    EXPECT_THROW(assetManager->Load("bad/path", false), std::runtime_error);
}

TEST_F(ConcaveColliderAssetManager_tests, Load_Collection_ReturnsTrue)
{
    std::array<std::string, 2u> paths{ConcavePath1, ConcavePath2};
    auto actual = assetManager->Load(paths, false);
    EXPECT_TRUE(actual);
}

TEST_F(ConcaveColliderAssetManager_tests, Unload_Loaded_ReturnsTrue)
{
    assetManager->Load(ConcavePath1, false);
    auto actual = assetManager->Unload(ConcavePath1);
    EXPECT_TRUE(actual);
}

TEST_F(ConcaveColliderAssetManager_tests, Unload_NotLoaded_ReturnsFalse)
{
    auto actual = assetManager->Unload(ConcavePath1);
    EXPECT_FALSE(actual);
}

TEST_F(ConcaveColliderAssetManager_tests, Unload_BadPath_ReturnsFalse)
{
    auto actual = assetManager->Unload("bad/path");
    EXPECT_FALSE(actual);
}

TEST_F(ConcaveColliderAssetManager_tests, IsLoaded_Loaded_ReturnsTrue)
{
    assetManager->Load(ConcavePath1, false);
    auto actual = assetManager->IsLoaded(ConcavePath1);
    EXPECT_TRUE(actual);
}

TEST_F(ConcaveColliderAssetManager_tests, IsLoaded_NotLoaded_ReturnsFalse)
{
    auto actual = assetManager->IsLoaded(ConcavePath1);
    EXPECT_FALSE(actual);
}

TEST_F(ConcaveColliderAssetManager_tests, IsLoaded_AfterUnload_ReturnsFalse)
{
    assetManager->Load(ConcavePath1, false);
    assetManager->Unload(ConcavePath1);
    auto actual = assetManager->IsLoaded(ConcavePath1);
    EXPECT_FALSE(actual);
}

TEST_F(ConcaveColliderAssetManager_tests, UnloadAll_HasAssets_RemovesAssets)
{
    std::array<std::string, 2u> paths{ConcavePath1, ConcavePath2};
    assetManager->Load(paths, false);
    assetManager->UnloadAll(AssetFlags::None);
    EXPECT_FALSE(assetManager->Unload(ConcavePath1));
    EXPECT_FALSE(assetManager->Unload(ConcavePath2));
}

TEST_F(ConcaveColliderAssetManager_tests, UnloadAll_Empty_Completes)
{
    assetManager->UnloadAll(AssetFlags::None);
}

TEST_F(ConcaveColliderAssetManager_tests, GetPath_Loaded_ReturnsPath)
{
    std::array<std::string, 2u> paths{ConcavePath1, ConcavePath2};
    assetManager->Load(paths, false);
    const auto& expected = paths.at(0);
    const auto view = assetManager->Acquire(expected);
    const auto actual = assetManager->GetPath(view.id);
    EXPECT_EQ(expected, actual);
}

TEST_F(ConcaveColliderAssetManager_tests, GetPath_NotLoaded_Throws)
{
    std::array<std::string, 2u> paths{ConcavePath1, ConcavePath2};
    assetManager->Load(paths, false);
    const auto& expected = paths.at(0);
    const auto view = assetManager->Acquire(expected);
    assetManager->UnloadAll();
    EXPECT_THROW(assetManager->GetPath(view.id), nc::NcError);
}
