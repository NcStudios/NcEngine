#include "gtest/gtest.h"
#include "asset/AssetData.h"
#include "assets/manager/TextureAssetManager.h"

#include <array>
#include <string>

using namespace nc;

const auto Texture_base = "texture_base.nca";
const auto Texture_normal = "texture_normal.nca";
const auto Texture_roughness = "texture_roughness.nca";
const uint32_t MaxTextures = 100;

class TextureAssetManager_tests : public ::testing::Test
{
    public:
        std::unique_ptr<TextureAssetManager> assetManager;

    protected:
        void SetUp() override
        {
            assetManager = std::make_unique<TextureAssetManager>(NC_TEST_COLLATERAL_DIRECTORY, MaxTextures);
        }

        void TearDown() override
        {
            assetManager = nullptr;
        }
};

TEST_F(TextureAssetManager_tests, Load_NotLoaded_ReturnsTrue)
{
    auto actual = assetManager->Load(Texture_base, false);
    EXPECT_TRUE(actual);
}

TEST_F(TextureAssetManager_tests, Load_IsLoaded_ReturnsFalse)
{
    assetManager->Load(Texture_base, false);
    auto actual = assetManager->Load(Texture_base, false);
    EXPECT_FALSE(actual);
}

TEST_F(TextureAssetManager_tests, Load_BadPath_Throws)
{
    EXPECT_THROW(assetManager->Load("bad/path", false), std::runtime_error);
}

TEST_F(TextureAssetManager_tests, Load_Collection_ReturnsTrue)
{
    std::array<std::string, 3u> paths {Texture_base, Texture_normal, Texture_roughness};
    auto actual = assetManager->Load(paths, false);
    EXPECT_TRUE(actual);
}

TEST_F(TextureAssetManager_tests, Load_Collection_OneAlreadyLoaded_IndexRemainsTheSame)
{
    std::array<std::string, 3u> paths {Texture_base, Texture_normal, Texture_roughness};
    assetManager->Load(paths, false);
    auto viewFirstLoad = assetManager->Acquire(Texture_base);

    std::array<std::string, 1u> paths2 {Texture_base};
    assetManager->Load(paths2, false);

    auto viewSecondLoad = assetManager->Acquire(Texture_base);
    EXPECT_EQ(viewFirstLoad.index, 0u);
    EXPECT_EQ(viewSecondLoad.index, 0u);
}

TEST_F(TextureAssetManager_tests, Unload_Loaded_ReturnsTrue)
{
    assetManager->Load(Texture_base, false);
    auto actual = assetManager->Unload(Texture_base);
    EXPECT_TRUE(actual);
}

TEST_F(TextureAssetManager_tests, Unload_NotLoaded_ReturnsFalse)
{
    auto actual = assetManager->Unload(Texture_base);
    EXPECT_FALSE(actual);
}

TEST_F(TextureAssetManager_tests, IsLoaded_Loaded_ReturnsTrue)
{
    assetManager->Load(Texture_base, false);
    auto actual = assetManager->IsLoaded(Texture_base);
    EXPECT_TRUE(actual);
}

TEST_F(TextureAssetManager_tests, IsLoaded_NotLoaded_ReturnsFalse)
{
    auto actual = assetManager->IsLoaded(Texture_base);
    EXPECT_FALSE(actual);
}

TEST_F(TextureAssetManager_tests, UnloadAll_HasAssets_RemovesAssets)
{
    std::array<std::string, 3u> paths {Texture_base, Texture_normal, Texture_roughness};
    assetManager->Load(paths, false);
    assetManager->UnloadAll(asset_flags_type flags = AssetFlags::None);
    EXPECT_FALSE(assetManager->IsLoaded(Texture_base));
    EXPECT_FALSE(assetManager->IsLoaded(Texture_normal));
    EXPECT_FALSE(assetManager->IsLoaded(Texture_roughness));
}

TEST_F(TextureAssetManager_tests, UnloadAll_Empty_Completes)
{
    assetManager->UnloadAll(asset_flags_type flags = AssetFlags::None);
}

TEST_F(TextureAssetManager_tests, Unload_FromBeginning_UpdatesAccesors)
{
    std::array<std::string, 3u> paths {Texture_base, Texture_normal, Texture_roughness};
    assetManager->Load(paths, false);
    assetManager->Unload(Texture_base);
    auto view1 = assetManager->Acquire(Texture_normal);
    auto view2 = assetManager->Acquire(Texture_roughness);
    EXPECT_EQ(view1.index, 0u);
    EXPECT_EQ(view2.index, 1u);
}

TEST_F(TextureAssetManager_tests, Unload_FromMiddle_UpdatesAccesors)
{
    std::array<std::string, 3u> paths {Texture_base, Texture_normal, Texture_roughness};
    assetManager->Load(paths, false);
    assetManager->Unload(Texture_normal);
    auto view1 = assetManager->Acquire(Texture_base);
    auto view2 = assetManager->Acquire(Texture_roughness);
    EXPECT_EQ(view1.index, 0u);
    EXPECT_EQ(view2.index, 1u);
}

TEST_F(TextureAssetManager_tests, Unload_FromEnd_AccessorsNotUpdated)
{
    std::array<std::string, 3u> paths {Texture_base, Texture_normal, Texture_roughness};
    assetManager->Load(paths, false);
    assetManager->Unload(Texture_roughness);
    auto view1 = assetManager->Acquire(Texture_base);
    auto view2 = assetManager->Acquire(Texture_normal);
    EXPECT_EQ(view1.index, 0u);
    EXPECT_EQ(view2.index, 1u);
}
