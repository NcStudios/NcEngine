#include "gtest/gtest.h"
#include "asset/AssetData.h"
#include "asset/manager/TextureAssetManager.h"

#include "ncasset/Assets.h"

#include <array>
#include <string>
#include <vector>

using namespace nc::asset;

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
    auto actual = assetManager->Load(Texture_base);
    EXPECT_TRUE(actual);
}

TEST_F(TextureAssetManager_tests, Load_IsLoaded_ReturnsFalse)
{
    assetManager->Load(Texture_base);
    auto actual = assetManager->Load(Texture_base);
    EXPECT_FALSE(actual);
}

TEST_F(TextureAssetManager_tests, Load_BadPath_Throws)
{
    EXPECT_THROW(assetManager->Load("bad/path"), std::runtime_error);
}

TEST_F(TextureAssetManager_tests, Load_Collection_ReturnsTrue)
{
    std::array<std::string, 3u> paths {Texture_base, Texture_normal, Texture_roughness};
    auto actual = assetManager->Load(paths);
    EXPECT_TRUE(actual);
}

TEST_F(TextureAssetManager_tests, Load_Collection_OneAlreadyLoaded_IndexRemainsTheSame)
{
    std::array<std::string, 3u> paths {Texture_base, Texture_normal, Texture_roughness};
    assetManager->Load(paths);
    auto viewFirstLoad = assetManager->Acquire(Texture_base);

    std::array<std::string, 1u> paths2 {Texture_base};
    assetManager->Load(paths2);

    auto viewSecondLoad = assetManager->Acquire(Texture_base);
    EXPECT_EQ(viewFirstLoad.index, 0u);
    EXPECT_EQ(viewSecondLoad.index, 0u);
}

TEST_F(TextureAssetManager_tests, Unload_Loaded_ReturnsTrue)
{
    assetManager->Load(Texture_base);
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
    assetManager->Load(Texture_base);
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
    assetManager->Load(paths);
    assetManager->UnloadAll();
    EXPECT_FALSE(assetManager->IsLoaded(Texture_base));
    EXPECT_FALSE(assetManager->IsLoaded(Texture_normal));
    EXPECT_FALSE(assetManager->IsLoaded(Texture_roughness));
}

TEST_F(TextureAssetManager_tests, UnloadAll_Empty_Completes)
{
    assetManager->UnloadAll();
}

TEST_F(TextureAssetManager_tests, Unload_FromBeginning_UpdatesAccesors)
{
    std::array<std::string, 3u> paths {Texture_base, Texture_normal, Texture_roughness};
    assetManager->Load(paths);
    assetManager->Unload(Texture_base);
    auto view1 = assetManager->Acquire(Texture_normal);
    auto view2 = assetManager->Acquire(Texture_roughness);
    EXPECT_EQ(view1.index, 0u);
    EXPECT_EQ(view2.index, 1u);
}

TEST_F(TextureAssetManager_tests, Unload_FromMiddle_UpdatesAccesors)
{
    std::array<std::string, 3u> paths {Texture_base, Texture_normal, Texture_roughness};
    assetManager->Load(paths);
    assetManager->Unload(Texture_normal);
    auto view1 = assetManager->Acquire(Texture_base);
    auto view2 = assetManager->Acquire(Texture_roughness);
    EXPECT_EQ(view1.index, 0u);
    EXPECT_EQ(view2.index, 1u);
}

TEST_F(TextureAssetManager_tests, Unload_FromEnd_AccessorsNotUpdated)
{
    std::array<std::string, 3u> paths {Texture_base, Texture_normal, Texture_roughness};
    assetManager->Load(paths);
    assetManager->Unload(Texture_roughness);
    auto view1 = assetManager->Acquire(Texture_base);
    auto view2 = assetManager->Acquire(Texture_normal);
    EXPECT_EQ(view1.index, 0u);
    EXPECT_EQ(view2.index, 1u);
}

TEST_F(TextureAssetManager_tests, GetPath_Loaded_ReturnsPath)
{
    std::array<std::string, 2u> paths{Texture_base, Texture_normal};
    assetManager->Load(paths);
    const auto& expected = paths.at(0);
    const auto view = assetManager->Acquire(expected);
    const auto actual = assetManager->GetPath(view.id);
    EXPECT_EQ(expected, actual);
}

TEST_F(TextureAssetManager_tests, GetPath_NotLoaded_Throws)
{
    std::array<std::string, 2u> paths{Texture_base, Texture_normal};
    assetManager->Load(paths);
    const auto& expected = paths.at(0);
    const auto view = assetManager->Acquire(expected);
    assetManager->UnloadAll();
    EXPECT_THROW(assetManager->GetPath(view.id), nc::NcError);
}

TEST_F(TextureAssetManager_tests, LoadFromMemory_NotLoaded_ReturnsTrue)
{
    auto texture = Texture{
        .format = TextureFormat::RGBA8_UNORM,
        .width = 2,
        .height = 2,
        .pixelData = std::vector<unsigned char>(16, 255)
    };
    auto actual = assetManager->LoadFromMemory("test_key", std::move(texture));
    EXPECT_TRUE(actual);
}

TEST_F(TextureAssetManager_tests, LoadFromMemory_IsLoaded_ReturnsFalse)
{
    auto texture1 = Texture{
        .format = TextureFormat::RGBA8_UNORM,
        .width = 2,
        .height = 2,
        .pixelData = std::vector<unsigned char>(16, 255)
    };
    auto texture2 = Texture{
        .format = TextureFormat::RGBA8_UNORM,
        .width = 2,
        .height = 2,
        .pixelData = std::vector<unsigned char>(16, 128)
    };
    assetManager->LoadFromMemory("test_key", std::move(texture1));
    auto actual = assetManager->LoadFromMemory("test_key", std::move(texture2));
    EXPECT_FALSE(actual);
}

TEST_F(TextureAssetManager_tests, LoadFromMemory_CanAcquire)
{
    auto texture = Texture{
        .format = TextureFormat::RGBA8_UNORM,
        .width = 2,
        .height = 2,
        .pixelData = std::vector<unsigned char>(16, 255)
    };
    assetManager->LoadFromMemory("test_key", std::move(texture));
    auto view = assetManager->Acquire("test_key");
    EXPECT_NE(view.id, NullAssetId);
    EXPECT_EQ(view.index, 0u);
}

TEST_F(TextureAssetManager_tests, LoadFromRGBA_NotLoaded_ReturnsTrue)
{
    std::vector<uint8_t> rgbaData(16, 255);
    auto actual = assetManager->LoadFromRGBA("test_key", rgbaData, 2, 2);
    EXPECT_TRUE(actual);
}

TEST_F(TextureAssetManager_tests, LoadFromRGBA_IsLoaded_ReturnsFalse)
{
    std::vector<uint8_t> rgbaData(16, 255);
    assetManager->LoadFromRGBA("test_key", rgbaData, 2, 2);
    auto actual = assetManager->LoadFromRGBA("test_key", rgbaData, 2, 2);
    EXPECT_FALSE(actual);
}

TEST_F(TextureAssetManager_tests, LoadFromRGBA_CanAcquire)
{
    std::vector<uint8_t> rgbaData(16, 255);
    assetManager->LoadFromRGBA("test_key", rgbaData, 2, 2);
    auto view = assetManager->Acquire("test_key");
    EXPECT_NE(view.id, NullAssetId);
    EXPECT_EQ(view.index, 0u);
}

TEST_F(TextureAssetManager_tests, LoadFromRGBA_WithFormat_CanAcquire)
{
    std::vector<uint8_t> rgbaData(16, 255);
    assetManager->LoadFromRGBA("test_key", rgbaData, 2, 2, TextureFormat::RGBA8_UNORM_SRGB);
    auto view = assetManager->Acquire("test_key");
    EXPECT_NE(view.id, NullAssetId);
}

TEST_F(TextureAssetManager_tests, LoadFromMemory_CanUnload)
{
    auto texture = Texture{
        .format = TextureFormat::RGBA8_UNORM,
        .width = 2,
        .height = 2,
        .pixelData = std::vector<unsigned char>(16, 255)
    };
    assetManager->LoadFromMemory("test_key", std::move(texture));
    auto actual = assetManager->Unload("test_key");
    EXPECT_TRUE(actual);
    EXPECT_FALSE(assetManager->IsLoaded("test_key"));
}

TEST_F(TextureAssetManager_tests, LoadFromMemory_MixedWithFileLoad_CorrectIndices)
{
    assetManager->Load(Texture_base);
    auto texture = Texture{
        .format = TextureFormat::RGBA8_UNORM,
        .width = 2,
        .height = 2,
        .pixelData = std::vector<unsigned char>(16, 255)
    };
    assetManager->LoadFromMemory("memory_texture", std::move(texture));
    assetManager->Load(Texture_normal);

    auto view1 = assetManager->Acquire(Texture_base);
    auto view2 = assetManager->Acquire("memory_texture");
    auto view3 = assetManager->Acquire(Texture_normal);

    EXPECT_EQ(view1.index, 0u);
    EXPECT_EQ(view2.index, 1u);
    EXPECT_EQ(view3.index, 2u);
}

TEST_F(TextureAssetManager_tests, GetAllLoaded_Default_IncludesRuntimeTextures)
{
    assetManager->Load(Texture_base);
    std::vector<uint8_t> rgbaData(16, 255);
    assetManager->LoadFromRGBA("runtime_tex", rgbaData, 2, 2);

    auto all = assetManager->GetAllLoaded();
    EXPECT_EQ(all.size(), 2u);
}

TEST_F(TextureAssetManager_tests, GetAllLoaded_SerializableOnly_ExcludesRuntimeTextures)
{
    assetManager->Load(Texture_base);
    std::vector<uint8_t> rgbaData(16, 255);
    assetManager->LoadFromRGBA("runtime_tex", rgbaData, 2, 2);

    auto serializable = assetManager->GetAllLoaded(true);
    EXPECT_EQ(serializable.size(), 1u);
    EXPECT_EQ(serializable[0], Texture_base);
}

TEST_F(TextureAssetManager_tests, GetAllLoaded_OnlyRuntimeTextures_SerializableReturnsEmpty)
{
    std::vector<uint8_t> rgbaData(16, 255);
    assetManager->LoadFromRGBA("runtime1", rgbaData, 2, 2);
    assetManager->LoadFromRGBA("runtime2", rgbaData, 2, 2);

    auto serializable = assetManager->GetAllLoaded(true);
    auto all = assetManager->GetAllLoaded(false);

    EXPECT_EQ(serializable.size(), 0u);
    EXPECT_EQ(all.size(), 2u);
}

TEST_F(TextureAssetManager_tests, IsLoaded_RuntimeTexture_ReturnsTrueWithOriginalKey)
{
    std::vector<uint8_t> rgbaData(16, 255);
    assetManager->LoadFromRGBA("my_runtime_tex", rgbaData, 2, 2);

    EXPECT_TRUE(assetManager->IsLoaded("my_runtime_tex"));
}

TEST_F(TextureAssetManager_tests, GetPath_RuntimeTexture_ReturnsUnprefixedKey)
{
    std::vector<uint8_t> rgbaData(16, 255);
    assetManager->LoadFromRGBA("test_runtime", rgbaData, 2, 2);

    auto view = assetManager->Acquire("test_runtime");
    auto path = assetManager->GetPath(view.id);

    EXPECT_FALSE(path.starts_with(RuntimeKey));
    EXPECT_NE(path.find("test_runtime"), std::string_view::npos);
}

TEST_F(TextureAssetManager_tests, UnloadAll_ClearsRuntimeTextures)
{
    assetManager->Load(Texture_base);
    std::vector<uint8_t> rgbaData(16, 255);
    assetManager->LoadFromRGBA("runtime_tex", rgbaData, 2, 2);

    assetManager->UnloadAll();

    EXPECT_FALSE(assetManager->IsLoaded(Texture_base));
    EXPECT_FALSE(assetManager->IsLoaded("runtime_tex"));
}

TEST_F(TextureAssetManager_tests, Unload_RuntimeTexture_DoesNotAffectFileTextures)
{
    assetManager->Load(Texture_base);
    std::vector<uint8_t> rgbaData(16, 255);
    assetManager->LoadFromRGBA("runtime_tex", rgbaData, 2, 2);

    assetManager->Unload("runtime_tex");

    EXPECT_TRUE(assetManager->IsLoaded(Texture_base));
    EXPECT_FALSE(assetManager->IsLoaded("runtime_tex"));
}

TEST_F(TextureAssetManager_tests, Unload_FileTexture_DoesNotAffectRuntimeTextures)
{
    assetManager->Load(Texture_base);
    std::vector<uint8_t> rgbaData(16, 255);
    assetManager->LoadFromRGBA("runtime_tex", rgbaData, 2, 2);

    assetManager->Unload(Texture_base);

    EXPECT_FALSE(assetManager->IsLoaded(Texture_base));
    EXPECT_TRUE(assetManager->IsLoaded("runtime_tex"));
}

TEST_F(TextureAssetManager_tests, LoadFromMemory_ExceedsMaxTextures_Throws)
{
    auto smallManager = std::make_unique<TextureAssetManager>(NC_TEST_COLLATERAL_DIRECTORY, 2);
    smallManager->Load(Texture_base);

    std::vector<uint8_t> rgbaData(16, 255);
    EXPECT_THROW(smallManager->LoadFromRGBA("runtime_tex", rgbaData, 2, 2), nc::NcError);
}

TEST_F(TextureAssetManager_tests, Acquire_NotLoaded_Throws)
{
    EXPECT_THROW(assetManager->Acquire("nonexistent"), nc::NcError);
}
