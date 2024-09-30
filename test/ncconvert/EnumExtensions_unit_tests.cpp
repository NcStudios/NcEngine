#include "gtest/gtest.h"
#include "utility/EnumExtensions.h"

#include "ncutility/NcError.h"

TEST(EnumExtensionsTest, ToAssetType_fromString_succeeds)
{
    EXPECT_EQ(nc::convert::ToAssetType("audio-clip"), nc::asset::AssetType::AudioClip);
    EXPECT_EQ(nc::convert::ToAssetType("concave-collider"), nc::asset::AssetType::ConcaveCollider);
    EXPECT_EQ(nc::convert::ToAssetType("cube-map"), nc::asset::AssetType::CubeMap);
    EXPECT_EQ(nc::convert::ToAssetType("hull-collider"), nc::asset::AssetType::HullCollider);
    EXPECT_EQ(nc::convert::ToAssetType("mesh"), nc::asset::AssetType::Mesh);
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
    EXPECT_EQ(nc::convert::ToString(nc::asset::AssetType::ConcaveCollider), "concave-collider");
    EXPECT_EQ(nc::convert::ToString(nc::asset::AssetType::CubeMap), "cube-map");
    EXPECT_EQ(nc::convert::ToString(nc::asset::AssetType::HullCollider), "hull-collider");
    EXPECT_EQ(nc::convert::ToString(nc::asset::AssetType::Mesh), "mesh");
    EXPECT_EQ(nc::convert::ToString(nc::asset::AssetType::SkeletalAnimation), "skeletal-animation");
    EXPECT_EQ(nc::convert::ToString(nc::asset::AssetType::Texture), "texture");
}

TEST(EnumExtensionsTest, ToString_badAssetType_throws)
{
    EXPECT_THROW(nc::convert::ToString(static_cast<nc::asset::AssetType>(999)), nc::NcError);
}
