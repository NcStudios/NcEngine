#include "gtest/gtest.h"
#include "asset/AssetData.h"
#include "asset/manager/ShapeKeyAnimationAssetManager.h"

#include <array>
#include <string>

using namespace nc::asset;

const auto CubeAnim = "cube.nca";
const auto PlaneAnim = "plane.nca";
const auto FlagAnim = "flag.nca";

class ShapeKeyAnimationAssetManager_tests : public ::testing::Test
{
    public:
        std::unique_ptr<ShapeKeyAnimationAssetManager> assetManager;

    protected:
        void SetUp() override
        {
            assetManager = std::make_unique<ShapeKeyAnimationAssetManager>(NC_TEST_COLLATERAL_DIRECTORY, 1000);
        }

        void TearDown() override
        {
            assetManager = nullptr;
        }
};

TEST_F(ShapeKeyAnimationAssetManager_tests, Load_NotLoaded_ReturnsTrue)
{
    auto actual = assetManager->Load(CubeAnim);
    EXPECT_TRUE(actual);
}

TEST_F(ShapeKeyAnimationAssetManager_tests, Load_IsLoaded_ReturnsFalse)
{
    assetManager->Load(CubeAnim);
    auto actual = assetManager->Load(CubeAnim);
    EXPECT_FALSE(actual);
}

TEST_F(ShapeKeyAnimationAssetManager_tests, Load_BadPath_Throws)
{
    EXPECT_THROW(assetManager->Load("bad/path"), std::runtime_error);
}

TEST_F(ShapeKeyAnimationAssetManager_tests, Load_Collection_ReturnsTrue)
{
    std::array<std::string, 3u> paths{CubeAnim, PlaneAnim, FlagAnim};
    auto actual = assetManager->Load(paths);
    EXPECT_TRUE(actual);
}

TEST_F(ShapeKeyAnimationAssetManager_tests, Load_Collection_OneAlreadyLoaded_IndexRemainsTheSame)
{
    std::array<std::string, 3u> paths{CubeAnim, PlaneAnim, FlagAnim};
    assetManager->Load(paths);
    auto viewFirstLoad = assetManager->Acquire(CubeAnim);

    std::array<std::string, 1u> repeat{CubeAnim};
    assetManager->Load(repeat);

    auto viewSecondLoad = assetManager->Acquire(CubeAnim);
    EXPECT_EQ(viewFirstLoad.index, 0u);
    EXPECT_EQ(viewSecondLoad.index, 0u);
}

TEST_F(ShapeKeyAnimationAssetManager_tests, Unload_Loaded_ReturnsTrue)
{
    assetManager->Load(CubeAnim);
    auto actual = assetManager->Unload(CubeAnim);
    EXPECT_TRUE(actual);
}

TEST_F(ShapeKeyAnimationAssetManager_tests, Unload_NotLoaded_ReturnsFalse)
{
    auto actual = assetManager->Unload(CubeAnim);
    EXPECT_FALSE(actual);
}

TEST_F(ShapeKeyAnimationAssetManager_tests, IsLoaded_Loaded_ReturnsTrue)
{
    assetManager->Load(CubeAnim);
    auto actual = assetManager->IsLoaded(CubeAnim);
    EXPECT_TRUE(actual);
}

TEST_F(ShapeKeyAnimationAssetManager_tests, IsLoaded_NotLoaded_ReturnsFalse)
{
    auto actual = assetManager->IsLoaded(CubeAnim);
    EXPECT_FALSE(actual);
}

TEST_F(ShapeKeyAnimationAssetManager_tests, UnloadAll_HasAssets_RemovesAssets)
{
    std::array<std::string, 3u> paths{CubeAnim, PlaneAnim, FlagAnim};
    assetManager->Load(paths);
    assetManager->UnloadAll();
    EXPECT_FALSE(assetManager->IsLoaded(CubeAnim));
    EXPECT_FALSE(assetManager->IsLoaded(PlaneAnim));
    EXPECT_FALSE(assetManager->IsLoaded(FlagAnim));
}

TEST_F(ShapeKeyAnimationAssetManager_tests, UnloadAll_Empty_Completes)
{
    assetManager->UnloadAll();
}

TEST_F(ShapeKeyAnimationAssetManager_tests, Unload_FromBeginning_UpdatesAccessors)
{
    std::array<std::string, 3u> paths{CubeAnim, PlaneAnim, FlagAnim};
    assetManager->Load(paths);
    assetManager->Unload(CubeAnim);
    auto view1 = assetManager->Acquire(PlaneAnim);
    auto view2 = assetManager->Acquire(FlagAnim);
    EXPECT_EQ(view1.index, 0u);
    EXPECT_EQ(view2.index, 1u);
}

TEST_F(ShapeKeyAnimationAssetManager_tests, Unload_FromMiddle_UpdatesAccessors)
{
    std::array<std::string, 3u> paths{CubeAnim, PlaneAnim, FlagAnim};
    assetManager->Load(paths);
    assetManager->Unload(PlaneAnim);
    auto view1 = assetManager->Acquire(CubeAnim);
    auto view2 = assetManager->Acquire(FlagAnim);
    EXPECT_EQ(view1.index, 0u);
    EXPECT_EQ(view2.index, 1u);
}

TEST_F(ShapeKeyAnimationAssetManager_tests, Unload_FromEnd_AccessorsNotUpdated)
{
    std::array<std::string, 3u> paths{CubeAnim, PlaneAnim, FlagAnim};
    assetManager->Load(paths);
    assetManager->Unload(FlagAnim);
    auto view1 = assetManager->Acquire(CubeAnim);
    auto view2 = assetManager->Acquire(PlaneAnim);
    EXPECT_EQ(view1.index, 0u);
    EXPECT_EQ(view2.index, 1u);
}

TEST_F(ShapeKeyAnimationAssetManager_tests, GetPath_Loaded_ReturnsPath)
{
    std::array<std::string, 2u> paths{CubeAnim, PlaneAnim};
    assetManager->Load(paths);
    const auto& expected = paths.at(0);
    const auto view = assetManager->Acquire(expected);
    const auto actual = assetManager->GetPath(view.id);
    EXPECT_EQ(expected, actual);
}

TEST_F(ShapeKeyAnimationAssetManager_tests, GetPath_NotLoaded_Throws)
{
    std::array<std::string, 2u> paths{CubeAnim, PlaneAnim};
    assetManager->Load(paths);
    const auto& expected = paths.at(0);
    const auto view = assetManager->Acquire(expected);
    assetManager->UnloadAll();
    EXPECT_THROW(assetManager->GetPath(view.id), nc::NcError);
}