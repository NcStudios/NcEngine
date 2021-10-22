#include "gtest/gtest.h"
#include "assets/ConvexHullAssetManager.h"

using namespace nc;

const auto HullPath1 = std::string{NC_TEST_COLLATERAL_DIRECTORY} + "/convex_hull1.nca";
const auto HullPath2 = std::string{NC_TEST_COLLATERAL_DIRECTORY} + "/convex_hull2.nca";

class ConvexHullAssetManager_unit_tests : public ::testing::Test
{
    public:
        std::unique_ptr<ConvexHullAssetManager> assetManager;

    protected:
        void SetUp() override
        {
            assetManager = std::make_unique<ConvexHullAssetManager>();
        }

        void TearDown() override
        {
            assetManager = nullptr;
        }
};

TEST_F(ConvexHullAssetManager_unit_tests, Load_NotLoaded_ReturnsTrue)
{
    auto actual = assetManager->Load(HullPath1);
    EXPECT_TRUE(actual);
}

TEST_F(ConvexHullAssetManager_unit_tests, Load_Loaded_ReturnsFalse)
{
    assetManager->Load(HullPath1);
    auto actual = assetManager->Load(HullPath1);
    EXPECT_FALSE(actual);
}

TEST_F(ConvexHullAssetManager_unit_tests, Load_BadPath_Throws)
{
    EXPECT_THROW(assetManager->Load("bad/path"), std::runtime_error);
}

TEST_F(ConvexHullAssetManager_unit_tests, Load_Collection_ReturnsTrue)
{
    std::array<std::string, 2u> paths{HullPath1, HullPath2};
    auto actual = assetManager->Load(paths);
    EXPECT_TRUE(actual);
}

TEST_F(ConvexHullAssetManager_unit_tests, Unload_Loaded_ReturnsTrue)
{
    assetManager->Load(HullPath1);
    auto actual = assetManager->Unload(HullPath1);
    EXPECT_TRUE(actual);
}

TEST_F(ConvexHullAssetManager_unit_tests, Unload_NotLoaded_ReturnsFalse)
{
    auto actual = assetManager->Unload(HullPath1);
    EXPECT_FALSE(actual);
}

TEST_F(ConvexHullAssetManager_unit_tests, Unload_BadPath_ReturnsFalse)
{
    auto actual = assetManager->Unload("bad/path");
    EXPECT_FALSE(actual);
}

TEST_F(ConvexHullAssetManager_unit_tests, IsLoaded_Loaded_ReturnsTrue)
{
    assetManager->Load(HullPath1);
    auto actual = assetManager->IsLoaded(HullPath1);
    EXPECT_TRUE(actual);
}

TEST_F(ConvexHullAssetManager_unit_tests, IsLoaded_NotLoaded_ReturnsFalse)
{
    auto actual = assetManager->IsLoaded(HullPath1);
    EXPECT_FALSE(actual);
}

TEST_F(ConvexHullAssetManager_unit_tests, IsLoaded_AfterUnload_ReturnsFalse)
{
    assetManager->Load(HullPath1);
    assetManager->Unload(HullPath1);
    auto actual = assetManager->IsLoaded(HullPath1);
    EXPECT_FALSE(actual);
}

TEST_F(ConvexHullAssetManager_unit_tests, UnloadAll_HasAssets_RemovesAssets)
{
    std::array<std::string, 2u> paths{HullPath1, HullPath2};
    assetManager->Load(paths);
    assetManager->UnloadAll();
    EXPECT_FALSE(assetManager->Unload(HullPath1));
    EXPECT_FALSE(assetManager->Unload(HullPath2));
}

TEST_F(ConvexHullAssetManager_unit_tests, UnloadAll_Empty_Completes)
{
    assetManager->UnloadAll();
}

int main(int argc, char ** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}