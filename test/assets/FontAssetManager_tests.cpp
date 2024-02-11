#include "gtest/gtest.h"
#include "assets/manager/FontAssetManager.h"

#include "ncasset/Assets.h"
#include "imgui/imgui.h"

#include <array>
#include <string>

using namespace nc;

namespace nc::window
{
auto GetContentScale() -> Vector2
{
    return Vector2{1.0f, 1.0f};
}
} // namespace nc::window

const auto Font1 = Font{.path = "font.ttf", .size = 12.0f};
const auto Font2 = Font{.path = "font.ttf", .size = 16.0f};

class FontAssetManager_tests : public ::testing::Test
{
    public:
        std::unique_ptr<FontAssetManager> assetManager;

    protected:
        void SetUp() override
        {
            assetManager = std::make_unique<FontAssetManager>(NC_TEST_COLLATERAL_DIRECTORY);
        }

        void TearDown() override
        {
            assetManager = nullptr;
        }
};

TEST_F(FontAssetManager_tests, Load_NotLoaded_ReturnsTrue)
{
    auto actual = assetManager->Load(Font1, false);
    EXPECT_TRUE(actual);
}

TEST_F(FontAssetManager_tests, Load_Loaded_ReturnsFalse)
{
    assetManager->Load(Font1, false);
    auto actual = assetManager->Load(Font1, false);
    EXPECT_FALSE(actual);
}

TEST_F(FontAssetManager_tests, Load_BadPath_Throws)
{
    EXPECT_THROW(assetManager->Load(Font{"bad/path", 12.0f}, false), std::runtime_error);
}

TEST_F(FontAssetManager_tests, Load_Collection_ReturnsTrue)
{
    std::array<Font, 2u> paths{Font1, Font2};
    auto actual = assetManager->Load(paths, false);
    EXPECT_TRUE(actual);
}

TEST_F(FontAssetManager_tests, Unload_Loaded_ReturnsTrue)
{
    assetManager->Load(Font1, false);
    auto actual = assetManager->Unload(Font1);
    EXPECT_TRUE(actual);
}

TEST_F(FontAssetManager_tests, Unload_NotLoaded_ReturnsFalse)
{
    auto actual = assetManager->Unload(Font1);
    EXPECT_FALSE(actual);
}

TEST_F(FontAssetManager_tests, Unload_BadPath_ReturnsFalse)
{
    auto actual = assetManager->Unload(Font{"bad/path", 12.0f});
    EXPECT_FALSE(actual);
}

TEST_F(FontAssetManager_tests, IsLoaded_Loaded_ReturnsTrue)
{
    assetManager->Load(Font1, false);
    auto actual = assetManager->IsLoaded(Font1);
    EXPECT_TRUE(actual);
}

TEST_F(FontAssetManager_tests, IsLoaded_NotLoaded_ReturnsFalse)
{
    auto actual = assetManager->IsLoaded(Font1);
    EXPECT_FALSE(actual);
}

TEST_F(FontAssetManager_tests, IsLoaded_AfterUnload_ReturnsFalse)
{
    assetManager->Load(Font1, false);
    assetManager->Unload(Font1);
    auto actual = assetManager->IsLoaded(Font1);
    EXPECT_FALSE(actual);
}

TEST_F(FontAssetManager_tests, UnloadAll_HasAssets_RemovesAssets)
{
    std::array<Font, 2u> paths{Font1, Font2};
    assetManager->Load(paths, false);
    assetManager->UnloadAll(AssetFlags::None);
    EXPECT_FALSE(assetManager->Unload(Font1));
    EXPECT_FALSE(assetManager->Unload(Font2));
}

TEST_F(FontAssetManager_tests, UnloadAll_Empty_Completes)
{
    assetManager->UnloadAll(AssetFlags::None);
}
