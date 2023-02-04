#include "gtest/gtest.h"
#include "assets/ConvexHullAssetManager.h"

#include "ncasset/Assets.h"

#include <array>
#include <string>

using namespace nc;

const auto HullPath1 = "hull_collider1.nca";
const auto HullPath2 = "hull_collider2.nca";

class ConvexHullAssetManager_tests : public ::testing::Test
{
    public:
        std::unique_ptr<ConvexHullAssetManager> assetManager;

    protected:
        void SetUp() override
        {
            assetManager = std::make_unique<ConvexHullAssetManager>(NC_TEST_COLLATERAL_DIRECTORY);
        }

        void TearDown() override
        {
            assetManager = nullptr;
        }
};

TEST_F(ConvexHullAssetManager_tests, Load_NotLoaded_ReturnsTrue)
{
    auto actual = assetManager->Load(HullPath1, false);
    EXPECT_TRUE(actual);
}

TEST_F(ConvexHullAssetManager_tests, Load_Loaded_ReturnsFalse)
{
    assetManager->Load(HullPath1, false);
    auto actual = assetManager->Load(HullPath1, false);
    EXPECT_FALSE(actual);
}

TEST_F(ConvexHullAssetManager_tests, Load_BadPath_Throws)
{
    EXPECT_THROW(assetManager->Load("bad/path", false), std::runtime_error);
}

TEST_F(ConvexHullAssetManager_tests, Load_Collection_ReturnsTrue)
{
    std::array<std::string, 2u> paths{HullPath1, HullPath2};
    auto actual = assetManager->Load(paths, false);
    EXPECT_TRUE(actual);
}

TEST_F(ConvexHullAssetManager_tests, Unload_Loaded_ReturnsTrue)
{
    assetManager->Load(HullPath1, false);
    auto actual = assetManager->Unload(HullPath1);
    EXPECT_TRUE(actual);
}

TEST_F(ConvexHullAssetManager_tests, Unload_NotLoaded_ReturnsFalse)
{
    auto actual = assetManager->Unload(HullPath1);
    EXPECT_FALSE(actual);
}

TEST_F(ConvexHullAssetManager_tests, Unload_BadPath_ReturnsFalse)
{
    auto actual = assetManager->Unload("bad/path");
    EXPECT_FALSE(actual);
}

TEST_F(ConvexHullAssetManager_tests, IsLoaded_Loaded_ReturnsTrue)
{
    assetManager->Load(HullPath1, false);
    auto actual = assetManager->IsLoaded(HullPath1);
    EXPECT_TRUE(actual);
}

TEST_F(ConvexHullAssetManager_tests, IsLoaded_NotLoaded_ReturnsFalse)
{
    auto actual = assetManager->IsLoaded(HullPath1);
    EXPECT_FALSE(actual);
}

TEST_F(ConvexHullAssetManager_tests, IsLoaded_AfterUnload_ReturnsFalse)
{
    assetManager->Load(HullPath1, false);
    assetManager->Unload(HullPath1);
    auto actual = assetManager->IsLoaded(HullPath1);
    EXPECT_FALSE(actual);
}

TEST_F(ConvexHullAssetManager_tests, UnloadAll_HasAssets_RemovesAssets)
{
    std::array<std::string, 2u> paths{HullPath1, HullPath2};
    assetManager->Load(paths, false);
    assetManager->UnloadAll();
    EXPECT_FALSE(assetManager->Unload(HullPath1));
    EXPECT_FALSE(assetManager->Unload(HullPath2));
}

TEST_F(ConvexHullAssetManager_tests, UnloadAll_Empty_Completes)
{
    assetManager->UnloadAll();
}
