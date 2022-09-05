#include "gtest/gtest.h"
#include "assets/CubeMapAssetManager.h"

#include <array>
#include <string>

#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

using namespace nc;

const auto skybox1 = "skybox1/skybox1.nca";
const auto skybox2 = "skybox2/skybox2.nca";
const auto skybox3 = "skybox3/skybox3.nca";
const uint32_t MaxTextures = 100;

class CubeMapAssetManager_tests : public ::testing::Test
{
    public:
        std::unique_ptr<CubeMapAssetManager> assetManager;

    protected:
        void SetUp() override
        {
            assetManager = std::make_unique<CubeMapAssetManager>(NC_TEST_COLLATERAL_DIRECTORY, MaxTextures);
        }

        void TearDown() override
        {
            assetManager = nullptr;
        }
};

TEST_F(CubeMapAssetManager_tests, Load_NotLoaded_ReturnsTrue)
{
    auto actual = assetManager->Load(skybox1, false);
    EXPECT_TRUE(actual);
}

TEST_F(CubeMapAssetManager_tests, Load_IsLoaded_ReturnsFalse)
{
    assetManager->Load(skybox1, false);
    auto actual = assetManager->Load(skybox1, false);
    EXPECT_FALSE(actual);
}

TEST_F(CubeMapAssetManager_tests, Load_BadPath_Throws)
{
    EXPECT_THROW(assetManager->Load("bad/path", false), std::runtime_error);
}

TEST_F(CubeMapAssetManager_tests, Load_Collection_ReturnsTrue)
{
    std::array<std::string, 3u> paths {skybox1, skybox2, skybox3};
    auto actual = assetManager->Load(paths, false);
    EXPECT_TRUE(actual);
}

TEST_F(CubeMapAssetManager_tests, Load_Collection_OneAlreadyLoaded_IndexRemainsTheSame)
{
    std::array<std::string, 3u> paths {skybox1, skybox2, skybox3};
    assetManager->Load(paths, false);
    auto viewFirstLoad = assetManager->Acquire(skybox1);

    std::array<std::string, 1u> paths2 {skybox1};
    assetManager->Load(paths2, false);

    auto viewSecondLoad = assetManager->Acquire(skybox1);
    EXPECT_EQ(viewFirstLoad.index, 0u);
    EXPECT_EQ(viewSecondLoad.index, 0u);
}

TEST_F(CubeMapAssetManager_tests, Unload_Loaded_ReturnsTrue)
{
    assetManager->Load(skybox1, false);
    auto actual = assetManager->Unload(skybox1);
    EXPECT_TRUE(actual);
}

TEST_F(CubeMapAssetManager_tests, Unload_NotLoaded_ReturnsFalse)
{
    auto actual = assetManager->Unload(skybox1);
    EXPECT_FALSE(actual);
}

TEST_F(CubeMapAssetManager_tests, IsLoaded_Loaded_ReturnsTrue)
{
    assetManager->Load(skybox1, false);
    auto actual = assetManager->IsLoaded(skybox1);
    EXPECT_TRUE(actual);
}

TEST_F(CubeMapAssetManager_tests, IsLoaded_NotLoaded_ReturnsFalse)
{
    auto actual = assetManager->IsLoaded(skybox1);
    EXPECT_FALSE(actual);
}

TEST_F(CubeMapAssetManager_tests, UnloadAll_HasAssets_RemovesAssets)
{
    std::array<std::string, 3u> paths {skybox1, skybox2, skybox3};
    assetManager->Load(paths, false);
    assetManager->UnloadAll();
    EXPECT_FALSE(assetManager->IsLoaded(skybox1));
    EXPECT_FALSE(assetManager->IsLoaded(skybox2));
    EXPECT_FALSE(assetManager->IsLoaded(skybox3));
}

TEST_F(CubeMapAssetManager_tests, UnloadAll_Empty_Completes)
{
    assetManager->UnloadAll();
}

TEST_F(CubeMapAssetManager_tests, Unload_FromBeginning_UpdatesAccesors)
{
    std::array<std::string, 3u> paths {skybox1, skybox2, skybox3};
    assetManager->Load(paths, false);
    assetManager->Unload(skybox1);
    auto view1 = assetManager->Acquire(skybox2);
    auto view2 = assetManager->Acquire(skybox3);
    EXPECT_EQ(view1.index, 0u);
    EXPECT_EQ(view2.index, 1u);
}

TEST_F(CubeMapAssetManager_tests, Unload_FromMiddle_UpdatesAccesors)
{
    std::array<std::string, 3u> paths {skybox1, skybox2, skybox3};
    assetManager->Load(paths, false);
    assetManager->Unload(skybox2);
    auto view1 = assetManager->Acquire(skybox1);
    auto view2 = assetManager->Acquire(skybox3);
    EXPECT_EQ(view1.index, 0u);
    EXPECT_EQ(view2.index, 1u);
}

TEST_F(CubeMapAssetManager_tests, Unload_FromEnd_AccessorsNotUpdated)
{
    std::array<std::string, 3u> paths {skybox1, skybox2, skybox3};
    assetManager->Load(paths, false);
    assetManager->Unload(skybox3);
    auto view1 = assetManager->Acquire(skybox1);
    auto view2 = assetManager->Acquire(skybox2);
    EXPECT_EQ(view1.index, 0u);
    EXPECT_EQ(view2.index, 1u);
}

int main(int argc, char ** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}