#include "gtest/gtest.h"
#include "utility/EnumExtensions.h"

#include "ncutility/NcError.h"

TEST(EnumExtensionsTest, ToAssetType_fromString_succeeds)
{
    EXPECT_EQ(nc::convert::ToAssetType("audio-clip"), nc::asset::AssetType::AudioClip);
    EXPECT_EQ(nc::convert::ToAssetType("convex-hull"), nc::asset::AssetType::ConvexHull);
    EXPECT_EQ(nc::convert::ToAssetType("cube-map"), nc::asset::AssetType::CubeMap);
    EXPECT_EQ(nc::convert::ToAssetType("mesh"), nc::asset::AssetType::Mesh);
    EXPECT_EQ(nc::convert::ToAssetType("mesh-collider"), nc::asset::AssetType::MeshCollider);
    EXPECT_EQ(nc::convert::ToAssetType("skeletal-animation"), nc::asset::AssetType::SkeletalAnimation);
    EXPECT_EQ(nc::convert::ToAssetType("texture"), nc::asset::AssetType::Texture);
}

TEST(EnumExtensionsTest, ToAssetType_badString_throws)
{
    EXPECT_THROW(nc::convert::ToAssetType("not-an-asset"), nc::NcError);
}

TEST(EnumExtensionsTest, ToString_fromAssetType_succeeds)
{
    EXPECT_EQ(nc::convert::ToString(nc::asset::AssetType::AudioClip), "audio-clip");
    EXPECT_EQ(nc::convert::ToString(nc::asset::AssetType::ConvexHull), "convex-hull");
    EXPECT_EQ(nc::convert::ToString(nc::asset::AssetType::CubeMap), "cube-map");
    EXPECT_EQ(nc::convert::ToString(nc::asset::AssetType::Mesh), "mesh");
    EXPECT_EQ(nc::convert::ToString(nc::asset::AssetType::MeshCollider), "mesh-collider");
    EXPECT_EQ(nc::convert::ToString(nc::asset::AssetType::SkeletalAnimation), "skeletal-animation");
    EXPECT_EQ(nc::convert::ToString(nc::asset::AssetType::Texture), "texture");
}

TEST(EnumExtensionsTest, ToString_badAssetType_throws)
{
    EXPECT_THROW(nc::convert::ToString(static_cast<nc::asset::AssetType>(999)), nc::NcError);
}

TEST(EnumExtensionsTest, IsCompressedTextureFormat_returnsExpectedValue)
{
    EXPECT_FALSE(nc::convert::IsCompressedTextureFormat(nc::asset::TextureFormat::RGBA8_UNORM_SRGB));
    EXPECT_FALSE(nc::convert::IsCompressedTextureFormat(nc::asset::TextureFormat::RGBA8_UNORM_SRGB));
    EXPECT_TRUE(nc::convert::IsCompressedTextureFormat(nc::asset::TextureFormat::BC1_UNORM_SRGB));
    EXPECT_TRUE(nc::convert::IsCompressedTextureFormat(nc::asset::TextureFormat::BC1_UNORM));
    EXPECT_TRUE(nc::convert::IsCompressedTextureFormat(nc::asset::TextureFormat::BC3_UNORM_SRGB));
    EXPECT_TRUE(nc::convert::IsCompressedTextureFormat(nc::asset::TextureFormat::BC3_UNORM));
}

TEST(EnumExtensionsTest, TextureFormatHasAlpha_returnsExpectedValue)
{
    EXPECT_TRUE(nc::convert::TextureFormatHasAlpha(nc::asset::TextureFormat::RGBA8_UNORM_SRGB));
    EXPECT_TRUE(nc::convert::TextureFormatHasAlpha(nc::asset::TextureFormat::RGBA8_UNORM_SRGB));
    EXPECT_FALSE(nc::convert::TextureFormatHasAlpha(nc::asset::TextureFormat::BC1_UNORM_SRGB));
    EXPECT_FALSE(nc::convert::TextureFormatHasAlpha(nc::asset::TextureFormat::BC1_UNORM));
    EXPECT_TRUE(nc::convert::TextureFormatHasAlpha(nc::asset::TextureFormat::BC3_UNORM_SRGB));
    EXPECT_TRUE(nc::convert::TextureFormatHasAlpha(nc::asset::TextureFormat::BC3_UNORM));
}

TEST(EnumExtensionsTest, GetMinimumDimensions_returnsExpectedValue)
{
    EXPECT_EQ(nc::convert::GetMinimumDimension(nc::asset::TextureFormat::RGBA8_UNORM_SRGB), 1);
    EXPECT_EQ(nc::convert::GetMinimumDimension(nc::asset::TextureFormat::RGBA8_UNORM), 1);
    EXPECT_EQ(nc::convert::GetMinimumDimension(nc::asset::TextureFormat::BC1_UNORM_SRGB), 4);
    EXPECT_EQ(nc::convert::GetMinimumDimension(nc::asset::TextureFormat::BC1_UNORM), 4);
    EXPECT_EQ(nc::convert::GetMinimumDimension(nc::asset::TextureFormat::BC3_UNORM_SRGB), 4);
    EXPECT_EQ(nc::convert::GetMinimumDimension(nc::asset::TextureFormat::BC3_UNORM), 4);
}

TEST(EnumExtensionsTest, ToString_fromTextureFormat_succeeds)
{
    EXPECT_EQ(nc::convert::ToString(nc::asset::TextureFormat::UNKNOWN), "UNKNOWN");
    EXPECT_EQ(nc::convert::ToString(nc::asset::TextureFormat::RGBA8_UNORM_SRGB), "RGBA8_UNORM_SRGB");
    EXPECT_EQ(nc::convert::ToString(nc::asset::TextureFormat::RGBA8_UNORM), "RGBA8_UNORM");
    EXPECT_EQ(nc::convert::ToString(nc::asset::TextureFormat::BC1_UNORM_SRGB), "BC1_UNORM_SRGB");
    EXPECT_EQ(nc::convert::ToString(nc::asset::TextureFormat::BC1_UNORM), "BC1_UNORM");
    EXPECT_EQ(nc::convert::ToString(nc::asset::TextureFormat::BC3_UNORM_SRGB), "BC3_UNORM_SRGB");
    EXPECT_EQ(nc::convert::ToString(nc::asset::TextureFormat::BC3_UNORM), "BC3_UNORM");
}
