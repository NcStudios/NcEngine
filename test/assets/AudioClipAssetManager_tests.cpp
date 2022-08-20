#include "gtest/gtest.h"
#include "assets/AudioClipAssetManager.h"

#include <array>
#include <string>

using namespace nc;

const auto SoundPath1 = "sound1.wav";
const auto SoundPath2 = "sound2.wav";

class AudioClipAssetManager_tests : public ::testing::Test
{
    public:
        std::unique_ptr<AudioClipAssetManager> assetManager;

    protected:
        void SetUp() override
        {
            assetManager = std::make_unique<AudioClipAssetManager>(NC_TEST_COLLATERAL_DIRECTORY);
        }

        void TearDown() override
        {
            assetManager = nullptr;
        }
};

TEST_F(AudioClipAssetManager_tests, Load_NotLoaded_ReturnsTrue)
{
    auto actual = assetManager->Load(SoundPath1, false);
    EXPECT_TRUE(actual);
}

TEST_F(AudioClipAssetManager_tests, Load_Loaded_ReturnsFalse)
{
    assetManager->Load(SoundPath1, false);
    auto actual = assetManager->Load(SoundPath1, false);
    EXPECT_FALSE(actual);
}

TEST_F(AudioClipAssetManager_tests, Load_BadPath_Throws)
{
    EXPECT_THROW(assetManager->Load("bad/path", false), std::runtime_error);
}

TEST_F(AudioClipAssetManager_tests, Load_Collection_ReturnsTrue)
{
    std::array<std::string, 2u> paths{SoundPath1, SoundPath2};
    auto actual = assetManager->Load(paths, false);
    EXPECT_TRUE(actual);
}

TEST_F(AudioClipAssetManager_tests, Unload_Loaded_ReturnsTrue)
{
    assetManager->Load(SoundPath1, false);
    auto actual = assetManager->Unload(SoundPath1);
    EXPECT_TRUE(actual);
}

TEST_F(AudioClipAssetManager_tests, Unload_NotLoaded_ReturnsFalse)
{
    auto actual = assetManager->Unload(SoundPath1);
    EXPECT_FALSE(actual);
}

TEST_F(AudioClipAssetManager_tests, Unload_BadPath_ReturnsFalse)
{
    auto actual = assetManager->Unload("bad/path");
    EXPECT_FALSE(actual);
}

TEST_F(AudioClipAssetManager_tests, IsLoaded_Loaded_ReturnsTrue)
{
    assetManager->Load(SoundPath1, false);
    auto actual = assetManager->IsLoaded(SoundPath1);
    EXPECT_TRUE(actual);
}

TEST_F(AudioClipAssetManager_tests, IsLoaded_NotLoaded_ReturnsFalse)
{
    auto actual = assetManager->IsLoaded(SoundPath1);
    EXPECT_FALSE(actual);
}

TEST_F(AudioClipAssetManager_tests, IsLoaded_AfterUnload_ReturnsFalse)
{
    assetManager->Load(SoundPath1, false);
    assetManager->Unload(SoundPath1);
    auto actual = assetManager->IsLoaded(SoundPath1);
    EXPECT_FALSE(actual);
}

TEST_F(AudioClipAssetManager_tests, UnloadAll_HasAssets_RemovesAssets)
{
    std::array<std::string, 2u> paths{SoundPath1, SoundPath2};
    assetManager->Load(paths, false);
    assetManager->UnloadAll();
    EXPECT_FALSE(assetManager->Unload(SoundPath1));
    EXPECT_FALSE(assetManager->Unload(SoundPath2));
}

TEST_F(AudioClipAssetManager_tests, UnloadAll_Empty_Completes)
{
    assetManager->UnloadAll();
}

int main(int argc, char ** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}