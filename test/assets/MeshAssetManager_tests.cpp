#include "gtest/gtest.h"
#include "assets/manager/MeshAssetManager.h"

#include <array>
#include <string>

using namespace nc;

const auto meshPath1 = "mesh1.nca";
const auto meshPath2 = "mesh2.nca";
const auto meshPath3 = "mesh3.nca";

class MeshAssetManager_tests : public ::testing::Test
{
    public:
        std::unique_ptr<MeshAssetManager> assetManager;

    protected:
        void SetUp() override
        {
            assetManager = std::make_unique<MeshAssetManager>(NC_TEST_COLLATERAL_DIRECTORY);
        }

        void TearDown() override
        {
            assetManager = nullptr;
        }
};

TEST_F(MeshAssetManager_tests, Load_NotLoaded_ReturnsTrue)
{
    auto actual = assetManager->Load(meshPath1, false);
    EXPECT_TRUE(actual);
}

TEST_F(MeshAssetManager_tests, Load_IsLoaded_ReturnsFalse)
{
    assetManager->Load(meshPath1, false);
    auto actual = assetManager->Load(meshPath1, false);
    EXPECT_FALSE(actual);
}

TEST_F(MeshAssetManager_tests, Load_BadPath_Throws)
{
    EXPECT_THROW(assetManager->Load("bad/path", false), std::runtime_error);
}

TEST_F(MeshAssetManager_tests, Load_Collection_ReturnsTrue)
{
    const auto paths = std::array<std::string, 3u>{meshPath1, meshPath2, meshPath3};
    auto actual = assetManager->Load(paths, false);
    EXPECT_TRUE(actual);
}

TEST_F(MeshAssetManager_tests, Load_Collection_OffsetsAccessors)
{
    const auto paths = std::array<std::string, 3u>{meshPath1, meshPath2, meshPath3};
    assetManager->Load(paths, false);

    auto view1 = assetManager->Acquire(meshPath1);
    auto view2 = assetManager->Acquire(meshPath2);
    auto view3 = assetManager->Acquire(meshPath3);

    EXPECT_EQ(view1.firstIndex, 0u);
    EXPECT_EQ(view1.firstIndex, 0u);
    EXPECT_EQ(view2.firstVertex, view1.vertexCount);
    EXPECT_EQ(view2.firstIndex, view1.indexCount);
    EXPECT_EQ(view3.firstVertex, view1.vertexCount + view2.vertexCount);
    EXPECT_EQ(view3.firstIndex, view1.indexCount + view2.indexCount);
}

TEST_F(MeshAssetManager_tests, Unload_Loaded_ReturnsTrue)
{
    assetManager->Load(meshPath1, false);
    auto actual = assetManager->Unload(meshPath1);
    EXPECT_TRUE(actual);
}

TEST_F(MeshAssetManager_tests, Unload_NotLoaded_ReturnsFalse)
{
    auto actual = assetManager->Unload(meshPath1);
    EXPECT_FALSE(actual);
}

TEST_F(MeshAssetManager_tests, IsLoaded_Loaded_ReturnsTrue)
{
    assetManager->Load(meshPath1, false);
    auto actual = assetManager->IsLoaded(meshPath1);
    EXPECT_TRUE(actual);
}

TEST_F(MeshAssetManager_tests, IsLoaded_NotLoaded_ReturnsFalse)
{
    auto actual = assetManager->IsLoaded(meshPath1);
    EXPECT_FALSE(actual);
}

TEST_F(MeshAssetManager_tests, UnloadAll_HasAssets_RemovesAssets)
{
    const auto paths = std::array<std::string, 3u>{meshPath1, meshPath2, meshPath3};
    assetManager->Load(paths, false);
    assetManager->UnloadAll(asset_flags_type flags = AssetFlags::None);
    EXPECT_FALSE(assetManager->IsLoaded(meshPath1));
    EXPECT_FALSE(assetManager->IsLoaded(meshPath2));
    EXPECT_FALSE(assetManager->IsLoaded(meshPath3));
}

TEST_F(MeshAssetManager_tests, UnloadAll_Empty_Completes)
{
    assetManager->UnloadAll(asset_flags_type flags = AssetFlags::None);
}

TEST_F(MeshAssetManager_tests, Unload_FromBeginning_UpdatesAccesors)
{
    const auto paths = std::array<std::string, 3u>{meshPath1, meshPath2, meshPath3};
    assetManager->Load(paths, false);
    assetManager->Unload(meshPath1);
    const auto view2 = assetManager->Acquire(meshPath2);
    const auto view3 = assetManager->Acquire(meshPath3);
    EXPECT_EQ(view2.firstIndex, 0u);
    EXPECT_EQ(view2.firstVertex, 0u);
    EXPECT_EQ(view3.firstIndex, view2.indexCount);
    EXPECT_EQ(view3.firstVertex, view2.vertexCount);
}

TEST_F(MeshAssetManager_tests, Unload_FromMiddle_UpdatesAccesors)
{
    const auto paths = std::array<std::string, 3u>{meshPath1, meshPath2, meshPath3};
    assetManager->Load(paths, false);
    assetManager->Unload(meshPath2);
    const auto view1 = assetManager->Acquire(meshPath1);
    const auto view3 = assetManager->Acquire(meshPath3);
    EXPECT_EQ(view1.firstVertex, 0u);
    EXPECT_EQ(view1.firstIndex, 0u);
    EXPECT_EQ(view3.firstVertex, view1.vertexCount);
    EXPECT_EQ(view3.firstIndex, view1.indexCount);
}

TEST_F(MeshAssetManager_tests, Unload_FromEnd_UpdatesAccesors)
{
    const auto paths = std::array<std::string, 3u>{meshPath1, meshPath2, meshPath3};
    assetManager->Load(paths, false);
    assetManager->Unload(meshPath3);
    const auto view1 = assetManager->Acquire(meshPath1);
    const auto view2 = assetManager->Acquire(meshPath2);
    EXPECT_EQ(view1.firstIndex, 0u);
    EXPECT_EQ(view1.firstVertex, 0u);
    EXPECT_EQ(view2.firstIndex, view1.indexCount);
    EXPECT_EQ(view2.firstVertex, view1.vertexCount);
}
