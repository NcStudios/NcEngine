#include <array>
#include <filesystem>

#include "gtest/gtest.h"

#include "assets/ShaderAssetManager.h"

using namespace nc;

class ShaderAssetManager_tests : public ::testing::Test
{
    public:
        std::unique_ptr<ShaderAssetManager> assetManager;

    protected:
        void SetUp() override
        {
            assetManager = std::make_unique<ShaderAssetManager>(NC_TEST_COLLATERAL_DIRECTORY);
        }

        void TearDown() override
        {
            assetManager = nullptr;
        }
};

TEST_F(ShaderAssetManager_tests, Load_NotLoaded_ReturnsTrue)
{
    const auto actual = assetManager->Load("shader1", false);
    EXPECT_TRUE(actual);
}

TEST_F(ShaderAssetManager_tests, Load_IsLoaded_ReturnsFalse)
{
    assetManager->Load("shader1", false);
    const auto actual = assetManager->Load("shader1", false);
    EXPECT_FALSE(actual);
}

TEST_F(ShaderAssetManager_tests, Load_BadPath_Throws)
{
    EXPECT_THROW(assetManager->Load("bad/path", false), std::runtime_error);
}

TEST_F(ShaderAssetManager_tests, Load_Collection_ReturnsTrue)
{
    std::array<std::string, 2u> paths {"shader1", "shader2"};
    const auto actual = assetManager->Load(paths, false);
    EXPECT_TRUE(actual);
}

TEST_F(ShaderAssetManager_tests, Unload_Loaded_ReturnsTrue)
{
    assetManager->Load("shader1", false);
    const auto actual = assetManager->Unload("shader1");
    EXPECT_TRUE(actual);
}

TEST_F(ShaderAssetManager_tests, Unload_NotLoaded_ReturnsFalse)
{
    const auto actual = assetManager->Unload("shader1");
    EXPECT_FALSE(actual);
}

TEST_F(ShaderAssetManager_tests, UnloadAll_HasAssets_RemovesAssets)
{
    std::array<std::string, 2u> paths {"shader1", "shader2"};
    assetManager->Load(paths, false);
    assetManager->UnloadAll();
    EXPECT_FALSE(assetManager->IsLoaded("shader1"));
    EXPECT_FALSE(assetManager->IsLoaded("shader2"));
}

int main(int argc, char ** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}