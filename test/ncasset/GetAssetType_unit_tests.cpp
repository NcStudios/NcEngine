#include "gtest/gtest.h"
#include "ncasset/NcaHeader.h"
#include "ncutility/NcError.h"

void SetMagicNumber(nc::asset::NcaHeader& header, std::string_view magicNumber)
{
    std::memcpy(header.magicNumber, magicNumber.data(), 4);
}

TEST(GetAssetTypeTests, ValidMagicNumber_succeeds)
{
    auto header = nc::asset::NcaHeader{};
    EXPECT_THROW(nc::asset::GetAssetType(header), nc::NcError);

    SetMagicNumber(header, nc::asset::MagicNumber::audioClip);
    EXPECT_EQ(nc::asset::AssetType::AudioClip, nc::asset::GetAssetType(header));

    SetMagicNumber(header, nc::asset::MagicNumber::concaveCollider);
    EXPECT_EQ(nc::asset::AssetType::ConcaveCollider, nc::asset::GetAssetType(header));

    SetMagicNumber(header, nc::asset::MagicNumber::cubeMap);
    EXPECT_EQ(nc::asset::AssetType::CubeMap, nc::asset::GetAssetType(header));

    SetMagicNumber(header, nc::asset::MagicNumber::hullCollider);
    EXPECT_EQ(nc::asset::AssetType::HullCollider, nc::asset::GetAssetType(header));

    SetMagicNumber(header, nc::asset::MagicNumber::mesh);
    EXPECT_EQ(nc::asset::AssetType::Mesh, nc::asset::GetAssetType(header));

    SetMagicNumber(header, nc::asset::MagicNumber::shader);
    EXPECT_EQ(nc::asset::AssetType::Shader, nc::asset::GetAssetType(header));

    SetMagicNumber(header, nc::asset::MagicNumber::texture);
    EXPECT_EQ(nc::asset::AssetType::Texture, nc::asset::GetAssetType(header));
}
