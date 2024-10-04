#include "gtest/gtest.h"
#include "Deserialize.h"
#include "builder/Serialize.h"
#include "utility/BlobSize.h"
#include "ncasset/Assets.h"

#include "ncmath/Math.h"
#include "ncutility/NcError.h"

#include <algorithm>
#include <sstream>

namespace nc::asset
{
bool operator==(const nc::asset::MeshVertex& lhs, const nc::asset::MeshVertex& rhs)
{
    return lhs.position == rhs.position &&
           lhs.normal == rhs.normal &&
           lhs.uv == rhs.uv &&
           lhs.tangent == rhs.tangent &&
           lhs.bitangent == rhs.bitangent;
}

bool Equals(const DirectX::XMMATRIX& lhs, const DirectX::XMMATRIX& rhs)
{
    DirectX::XMFLOAT4X4 lhsView;
    XMStoreFloat4x4(&lhsView, lhs);

    DirectX::XMFLOAT4X4 rhsView;
    XMStoreFloat4x4(&rhsView, rhs);

    return nc::FloatEqual(rhsView._11, lhsView._11) &&
           nc::FloatEqual(rhsView._12, lhsView._12) &&
           nc::FloatEqual(rhsView._13, lhsView._13) &&
           nc::FloatEqual(rhsView._14, lhsView._14) &&
           nc::FloatEqual(rhsView._21, lhsView._21) &&
           nc::FloatEqual(rhsView._22, lhsView._22) &&
           nc::FloatEqual(rhsView._23, lhsView._23) &&
           nc::FloatEqual(rhsView._24, lhsView._24) &&
           nc::FloatEqual(rhsView._31, lhsView._31) &&
           nc::FloatEqual(rhsView._32, lhsView._32) &&
           nc::FloatEqual(rhsView._33, lhsView._33) &&
           nc::FloatEqual(rhsView._34, lhsView._34) &&
           nc::FloatEqual(rhsView._41, lhsView._41) &&
           nc::FloatEqual(rhsView._42, lhsView._42) &&
           nc::FloatEqual(rhsView._43, lhsView._43) &&
           nc::FloatEqual(rhsView._44, lhsView._44);
}
}

TEST(AssetSerializationTest, HullCollider_roundTrip_succeeds)
{
    constexpr auto version = nc::asset::currentVersion;
    const auto expectedAsset = nc::asset::HullCollider{
        .extents = nc::Vector3{1.2f, 3.4f, 5.6f},
        .maxExtent = 42.42f,
        .vertices = std::vector<nc::Vector3>{
            nc::Vector3::Left(), nc::Vector3::Right(), nc::Vector3::Up(),
            nc::Vector3::Down(), nc::Vector3::Front(), nc::Vector3::Back()
        }
    };

    auto stream = std::stringstream{std::ios::in | std::ios::out | std::ios::binary};
    nc::convert::Serialize(stream, expectedAsset, version);
    const auto [actualHeader, actualAsset] = nc::asset::DeserializeHullCollider(stream);

    EXPECT_STREQ("HULL", actualHeader.magicNumber);
    EXPECT_EQ(version, actualHeader.version);
    EXPECT_EQ(nc::convert::GetBlobSize(expectedAsset), actualHeader.size);
    EXPECT_STREQ("NONE", actualHeader.compressionAlgorithm);

    EXPECT_EQ(expectedAsset.extents, actualAsset.extents);
    EXPECT_EQ(expectedAsset.maxExtent, actualAsset.maxExtent);
    ASSERT_EQ(expectedAsset.vertices.size(), actualAsset.vertices.size());
    EXPECT_TRUE(std::equal(expectedAsset.vertices.cbegin(),
                           expectedAsset.vertices.cend(),
                           actualAsset.vertices.cbegin()));
}

TEST(AssetSerializationTest, ConcaveCollider_roundTrip_succeeds)
{
    constexpr auto version = nc::asset::currentVersion;
    const auto expectedAsset = nc::asset::ConcaveCollider{
        .extents = nc::Vector3{-1.1f, 2.2f, -3.3f},
        .maxExtent = 123.321f,
        .triangles = std::vector<nc::Triangle>{
            nc::Triangle{nc::Vector3::Splat(1), nc::Vector3::Splat(2), nc::Vector3::Splat(3)},
            nc::Triangle{nc::Vector3::Splat(4), nc::Vector3::Splat(5), nc::Vector3::Splat(6)}
        }
    };

    auto stream = std::stringstream{std::ios::in | std::ios::out | std::ios::binary};
    nc::convert::Serialize(stream, expectedAsset, version);
    const auto [actualHeader, actualAsset] = nc::asset::DeserializeConcaveCollider(stream);

    EXPECT_STREQ("CONC", actualHeader.magicNumber);
    EXPECT_EQ(version, actualHeader.version);
    EXPECT_EQ(nc::convert::GetBlobSize(expectedAsset), actualHeader.size);
    EXPECT_STREQ("NONE", actualHeader.compressionAlgorithm);

    EXPECT_EQ(expectedAsset.extents, actualAsset.extents);
    EXPECT_EQ(expectedAsset.maxExtent, actualAsset.maxExtent);
    ASSERT_EQ(expectedAsset.triangles.size(), actualAsset.triangles.size());

    for(auto i = 0u; i < expectedAsset.triangles.size(); ++i)
    {
        const auto& expected = expectedAsset.triangles[i];
        const auto& actual = actualAsset.triangles[i];
        EXPECT_EQ(expected.a, actual.a);
        EXPECT_EQ(expected.b, actual.b);
        EXPECT_EQ(expected.c, actual.c);
    }
}

TEST(AssetSerializationTest, Mesh_hasBones_roundTrip_succeeds)
{
    constexpr auto version = nc::asset::currentVersion;
    auto expectedAsset = nc::asset::Mesh{
        .extents = nc::Vector3{-5.0f, 4.22f, 10.010101f},
        .maxExtent = 10.010101f,
        .vertices = std::vector<nc::asset::MeshVertex>{
            nc::asset::MeshVertex{nc::Vector3::Splat(0.0f),
                                  nc::Vector3::Splat(1.0f),
                                  nc::Vector2::Splat(2.0f),
                                  nc::Vector3::Splat(3.0f),
                                  nc::Vector3::Splat(4.0f),
                                  nc::Vector4::Splat(5.0f),
                                  std::array<uint32_t, 4>{6, 6, 6, 6}},
            nc::asset::MeshVertex{nc::Vector3::Splat(7.0f),
                                  nc::Vector3::Splat(8.0f),
                                  nc::Vector2::Splat(9.0f),
                                  nc::Vector3::Splat(10.0f),
                                  nc::Vector3::Splat(11.0f),
                                  nc::Vector4::Splat(12.0f),
                                  std::array<uint32_t, 4>{13, 13, 13, 13}},
            nc::asset::MeshVertex{nc::Vector3::Splat(14.0f),
                                  nc::Vector3::Splat(15.0f),
                                  nc::Vector2::Splat(16.0f),
                                  nc::Vector3::Splat(17.0f),
                                  nc::Vector3::Splat(18.0f),
                                  nc::Vector4::Splat(19.0f),
                                  std::array<uint32_t, 4>{20, 20, 20, 20}}
        },
        .indices = std::vector<uint32_t>{
            0, 1, 2,  1, 2, 0,  2, 0, 1
        },
        .bonesData = nc::asset::BonesData{
            .boneMapping = std::unordered_map<std::string, uint32_t>{},
            .vertexSpaceToBoneSpace = std::vector<nc::asset::VertexSpaceToBoneSpace>(0),
            .boneSpaceToParentSpace = std::vector<nc::asset::BoneSpaceToParentSpace>(0)
        }
    };

    // Can't initialize above due to internal compiler error in MS.
    expectedAsset.bonesData.value().vertexSpaceToBoneSpace.push_back(
        nc::asset::VertexSpaceToBoneSpace
        {
            .boneName = std::string("Bone0Bone0Bone0Bone0Bone0Bone0Bone0Bone0Bone0Bone0Bone0Bone0Bone0Bone0Bone0Bone0Bone0Bone0Bone0Bone0Bone0Bone0Bone0Bone0Bone0Bone0Bone0Bone0Bone0Bone0Bone0Bone0Bone0Bone0Bone0"),
            .transformationMatrix = DirectX::XMMATRIX
            {
                -1, -1, -1, -1,
                0, 0, 0, 0,
                0, 0, 0, 0,
                0, 0, 0, 0
            }
        });

    // Can't initialize above due to internal compiler error in MS.
    expectedAsset.bonesData.value().boneSpaceToParentSpace.push_back(
        nc::asset::BoneSpaceToParentSpace
        {
            .boneName = std::string("Bone0"),
            .transformationMatrix = DirectX::XMMATRIX
            {
                0, 0, 0, 1,
                0, 0, 1, 0,
                0, 1, 0, 0,
                1, 0, 0, 0
            },
            .numChildren = 0u,
            .indexOfFirstChild = 0u
        });

        // Can't initialize above due to internal compiler error in MS.
    expectedAsset.bonesData.value().boneMapping.emplace("Bone0Bone0Bone0Bone0Bone0Bone0Bone0Bone0Bone0Bone0Bone0Bone0Bone0Bone0Bone0Bone0Bone0Bone0Bone0Bone0Bone0Bone0Bone0Bone0Bone0Bone0Bone0Bone0Bone0Bone0Bone0Bone0Bone0Bone0Bone0", 0);

    auto stream = std::stringstream{std::ios::in | std::ios::out | std::ios::binary};
    nc::convert::Serialize(stream, expectedAsset, version);
    const auto [actualHeader, actualAsset] = nc::asset::DeserializeMesh(stream);

    EXPECT_STREQ("MESH", actualHeader.magicNumber);
    EXPECT_EQ(version, actualHeader.version);
    EXPECT_EQ(nc::convert::GetBlobSize(expectedAsset), actualHeader.size);
    EXPECT_STREQ("NONE", actualHeader.compressionAlgorithm);

    EXPECT_EQ(expectedAsset.extents, actualAsset.extents);
    EXPECT_EQ(expectedAsset.maxExtent, actualAsset.maxExtent);
    ASSERT_EQ(expectedAsset.vertices.size(), actualAsset.vertices.size());
    ASSERT_EQ(expectedAsset.indices.size(), actualAsset.indices.size());

    for(auto i = 0u; i < expectedAsset.vertices.size(); ++i)
    {
        const auto& e = expectedAsset.vertices[i];
        const auto& a = actualAsset.vertices[i];
        EXPECT_EQ(e, a);
    }

    EXPECT_TRUE(std::equal(expectedAsset.indices.cbegin(),
                           expectedAsset.indices.cend(),
                           actualAsset.indices.cbegin()));

    EXPECT_EQ(expectedAsset.bonesData.has_value(), actualAsset.bonesData.has_value());

    const auto& bonesData = expectedAsset.bonesData.value();
    EXPECT_EQ(bonesData.vertexSpaceToBoneSpace[0].boneName, "Bone0Bone0Bone0Bone0Bone0Bone0Bone0Bone0Bone0Bone0Bone0Bone0Bone0Bone0Bone0Bone0Bone0Bone0Bone0Bone0Bone0Bone0Bone0Bone0Bone0Bone0Bone0Bone0Bone0Bone0Bone0Bone0Bone0Bone0Bone0");
    EXPECT_EQ(bonesData.vertexSpaceToBoneSpace.size(), 1);
    EXPECT_EQ(bonesData.boneSpaceToParentSpace[0].boneName, "Bone0");
    EXPECT_TRUE(nc::asset::Equals(bonesData.vertexSpaceToBoneSpace[0].transformationMatrix, actualAsset.bonesData.value().vertexSpaceToBoneSpace[0].transformationMatrix));
    EXPECT_EQ(bonesData.boneSpaceToParentSpace.size(), 
              actualAsset.bonesData.value().boneSpaceToParentSpace.size());
}

TEST(AssetSerializationTest, Mesh_noBones_roundTrip_succeeds)
{
    constexpr auto version = nc::asset::currentVersion;
    const auto expectedAsset = nc::asset::Mesh{
        .extents = nc::Vector3{-5.0f, 4.22f, 10.010101f},
        .maxExtent = 10.010101f,
        .vertices = std::vector<nc::asset::MeshVertex>{
            nc::asset::MeshVertex{nc::Vector3::Splat(0.0f),
                                  nc::Vector3::Splat(1.0f),
                                  nc::Vector2::Splat(2.0f),
                                  nc::Vector3::Splat(3.0f),
                                  nc::Vector3::Splat(4.0f),
                                  nc::Vector4::Splat(5.0f),
                                  std::array<uint32_t, 4>{6, 6, 6, 6}},
            nc::asset::MeshVertex{nc::Vector3::Splat(7.0f),
                                  nc::Vector3::Splat(8.0f),
                                  nc::Vector2::Splat(9.0f),
                                  nc::Vector3::Splat(10.0f),
                                  nc::Vector3::Splat(11.0f),
                                  nc::Vector4::Splat(12.0f),
                                  std::array<uint32_t, 4>{13, 13, 13, 13}},
            nc::asset::MeshVertex{nc::Vector3::Splat(14.0f),
                                  nc::Vector3::Splat(15.0f),
                                  nc::Vector2::Splat(16.0f),
                                  nc::Vector3::Splat(17.0f),
                                  nc::Vector3::Splat(18.0f),
                                  nc::Vector4::Splat(19.0f),
                                  std::array<uint32_t, 4>{20, 20, 20, 20}}
        },
        .indices = std::vector<uint32_t>{
            0, 1, 2,  1, 2, 0,  2, 0, 1
        },
        .bonesData = std::nullopt
    };

    auto stream = std::stringstream{std::ios::in | std::ios::out | std::ios::binary};
    nc::convert::Serialize(stream, expectedAsset, version);
    const auto [actualHeader, actualAsset] = nc::asset::DeserializeMesh(stream);

    EXPECT_STREQ("MESH", actualHeader.magicNumber);
    EXPECT_EQ(version, actualHeader.version);
    EXPECT_EQ(nc::convert::GetBlobSize(expectedAsset), actualHeader.size);
    EXPECT_STREQ("NONE", actualHeader.compressionAlgorithm);

    EXPECT_EQ(expectedAsset.extents, actualAsset.extents);
    EXPECT_EQ(expectedAsset.maxExtent, actualAsset.maxExtent);
    ASSERT_EQ(expectedAsset.vertices.size(), actualAsset.vertices.size());
    ASSERT_EQ(expectedAsset.indices.size(), actualAsset.indices.size());

    for(auto i = 0u; i < expectedAsset.vertices.size(); ++i)
    {
        const auto& e = expectedAsset.vertices[i];
        const auto& a = actualAsset.vertices[i];
        EXPECT_EQ(e, a);
    }

    EXPECT_TRUE(std::equal(expectedAsset.indices.cbegin(),
                           expectedAsset.indices.cend(),
                           actualAsset.indices.cbegin()));

    EXPECT_EQ(expectedAsset.bonesData.has_value(), actualAsset.bonesData.has_value());
}

TEST(AssetSerializationTest, Texture_roundTrip_succeeds)
{
    constexpr auto version = nc::asset::currentVersion;
    const auto expectedAsset = nc::asset::Texture{
        .width = 2, .height = 2,
        .pixelData = std::vector<unsigned char>{
            0xA1, 0xA2, 0xA3, 0xA4,  0xB5, 0xB6, 0xB7, 0xB8,
            0xCA, 0xCB, 0xCC, 0xCD,  0xD0, 0xD1, 0xD2, 0xD3
        }
    };

    auto stream = std::stringstream{std::ios::in | std::ios::out | std::ios::binary};
    nc::convert::Serialize(stream, expectedAsset, version);
    const auto [actualHeader, actualAsset] = nc::asset::DeserializeTexture(stream);

    EXPECT_STREQ("TEXT", actualHeader.magicNumber);
    EXPECT_EQ(version, actualHeader.version);
    EXPECT_EQ(nc::convert::GetBlobSize(expectedAsset), actualHeader.size);
    EXPECT_STREQ("NONE", actualHeader.compressionAlgorithm);

    EXPECT_EQ(expectedAsset.width, actualAsset.width);
    EXPECT_EQ(expectedAsset.height, actualAsset.height);
    ASSERT_EQ(expectedAsset.pixelData.size(), actualAsset.pixelData.size());

    EXPECT_TRUE(std::equal(expectedAsset.pixelData.cbegin(),
                           expectedAsset.pixelData.cend(),
                           actualAsset.pixelData.cbegin()));
}

TEST(AssetSerializationTest, AudioClip_roundTrip_succeeds)
{
    constexpr auto version = nc::asset::currentVersion;
    const auto expectedAsset = nc::asset::AudioClip{
        .samplesPerChannel = 4ull,
        .leftChannel = std::vector<double>{0.0f, 0.5f, 1.0f, 0.5f},
        .rightChannel = std::vector<double>{0.0f, 0.25f, 0.75f, 1.0f}
    };

    auto stream = std::stringstream{std::ios::in | std::ios::out | std::ios::binary};
    nc::convert::Serialize(stream, expectedAsset, version);
    const auto [actualHeader, actualAsset] = nc::asset::DeserializeAudioClip(stream);

    EXPECT_STREQ("CLIP", actualHeader.magicNumber);
    EXPECT_EQ(version, actualHeader.version);
    EXPECT_EQ(nc::convert::GetBlobSize(expectedAsset), actualHeader.size);
    EXPECT_STREQ("NONE", actualHeader.compressionAlgorithm);

    EXPECT_EQ(expectedAsset.samplesPerChannel, actualAsset.samplesPerChannel);
    ASSERT_EQ(expectedAsset.leftChannel.size(), actualAsset.leftChannel.size());
    ASSERT_EQ(expectedAsset.rightChannel.size(), actualAsset.rightChannel.size());
    EXPECT_TRUE(std::equal(expectedAsset.leftChannel.cbegin(),
                           expectedAsset.leftChannel.cend(),
                           actualAsset.leftChannel.cbegin()));
    EXPECT_TRUE(std::equal(expectedAsset.rightChannel.cbegin(),
                           expectedAsset.rightChannel.cend(),
                           actualAsset.rightChannel.cbegin()));
}

TEST(AssetSerializationTest, CubeMap_roundTrip_succeeds)
{
    constexpr auto version = nc::asset::currentVersion;
    const auto expectedAsset = nc::asset::CubeMap{
        .faceSideLength = 1,
        .pixelData = std::vector<unsigned char>{
            0xA1, 0xA2, 0xA3, 0xA4, // front
            0xB1, 0xB2, 0xB3, 0xB4, // back
            0xC1, 0xC2, 0xC3, 0xC4, // up
            0xD1, 0xD2, 0xD3, 0xD4, // down
            0xE1, 0xE2, 0xE3, 0xE4, // right
            0xF1, 0xF2, 0xF3, 0xF4,  // left
        }
    };

    auto stream = std::stringstream{std::ios::in | std::ios::out | std::ios::binary};
    nc::convert::Serialize(stream, expectedAsset, version);
    const auto [actualHeader, actualAsset] = nc::asset::DeserializeCubeMap(stream);

    EXPECT_STREQ("CUBE", actualHeader.magicNumber);
    EXPECT_EQ(version, actualHeader.version);
    EXPECT_EQ(nc::convert::GetBlobSize(expectedAsset), actualHeader.size);
    EXPECT_STREQ("NONE", actualHeader.compressionAlgorithm);

    EXPECT_EQ(expectedAsset.faceSideLength, actualAsset.faceSideLength);
    ASSERT_EQ(expectedAsset.pixelData.size(), actualAsset.pixelData.size());

    EXPECT_TRUE(std::equal(expectedAsset.pixelData.cbegin(),
                           expectedAsset.pixelData.cend(),
                           actualAsset.pixelData.cbegin()));
}

TEST(AssetSerializationTest, SkeletalAnimation_roundTrip_succeeds)
{
    constexpr auto version = nc::asset::currentVersion;

    const auto firstBoneFrame = nc::asset::SkeletalAnimationFrames
    {
        std::vector<nc::asset::PositionFrame>
        {
            nc::asset::PositionFrame{0, nc::Vector3{0.0f, 0.0f, 0.0f}},
            nc::asset::PositionFrame{1, nc::Vector3{0.1f, 0.1f, 0.1f}},
            nc::asset::PositionFrame{2, nc::Vector3{0.2f, 0.2f, 0.2f}}
        },

        std::vector<nc::asset::RotationFrame>
        {
            nc::asset::RotationFrame{0, nc::Quaternion{1.0f, 1.0f, 1.0f, 1.0f}},
            nc::asset::RotationFrame{1, nc::Quaternion{1.1f, 1.1f, 1.1f, 1.0f}},
            nc::asset::RotationFrame{2, nc::Quaternion{1.2f, 1.2f, 1.2f, 1.0f}}
        },

        std::vector<nc::asset::ScaleFrame>
        {
            nc::asset::ScaleFrame{0, nc::Vector3{2.0f, 2.0f, 2.0f}},
            nc::asset::ScaleFrame{1, nc::Vector3{2.1f, 2.1f, 2.1f}},
            nc::asset::ScaleFrame{2, nc::Vector3{2.2f, 2.2f, 2.2f}}
        }
    };

    const auto secondBoneFrame = nc::asset::SkeletalAnimationFrames
    {
        std::vector<nc::asset::PositionFrame>
        {
            nc::asset::PositionFrame{0, nc::Vector3{3.0f, 3.0f, 3.0f}},
            nc::asset::PositionFrame{1, nc::Vector3{3.1f, 3.1f, 3.1f}},
            nc::asset::PositionFrame{2, nc::Vector3{3.2f, 3.2f, 3.2f}}
        },

        std::vector<nc::asset::RotationFrame>
        {
            nc::asset::RotationFrame{0, nc::Quaternion{4.0f, 4.0f, 4.0f, 4.0f}},
            nc::asset::RotationFrame{1, nc::Quaternion{4.1f, 4.1f, 4.1f, 4.0f}},
            nc::asset::RotationFrame{2, nc::Quaternion{4.2f, 4.2f, 4.2f, 4.0f}}
        },

        std::vector<nc::asset::ScaleFrame>
        {
            nc::asset::ScaleFrame{0, nc::Vector3{5.0f, 5.0f, 5.0f}},
            nc::asset::ScaleFrame{1, nc::Vector3{5.1f, 5.1f, 5.1f}},
            nc::asset::ScaleFrame{2, nc::Vector3{5.2f, 5.2f, 5.2f}}
        }
    };

    const auto skeletalAnimationFrames = std::unordered_map<std::string, nc::asset::SkeletalAnimationFrames>
    {
        {{std::string{"Bone0"}}, std::move(firstBoneFrame)},
        {{std::string{"Bone1"}}, std::move(secondBoneFrame)}
    };

    const auto expectedAsset = nc::asset::SkeletalAnimation{
        .name = "Test",
        .durationInTicks = 128,
        .ticksPerSecond = 64,
        .framesPerBone = std::move(skeletalAnimationFrames)
    };

    auto stream = std::stringstream{std::ios::in | std::ios::out | std::ios::binary};
    nc::convert::Serialize(stream, expectedAsset, version);
    const auto [actualHeader, actualAsset] = nc::asset::DeserializeSkeletalAnimation(stream);

    EXPECT_EQ(actualAsset.name, std::string{"Test"});
    EXPECT_EQ(actualAsset.durationInTicks, 128);
    EXPECT_EQ(actualAsset.ticksPerSecond, 64);
    EXPECT_EQ(actualAsset.framesPerBone.size(), 2);

    const auto& firstBoneFrames = actualAsset.framesPerBone.at("Bone0");
    EXPECT_EQ(firstBoneFrames.positionFrames.at(0).timeInTicks, 0);
    EXPECT_EQ(firstBoneFrames.positionFrames.at(1).timeInTicks, 1);
    EXPECT_EQ(firstBoneFrames.positionFrames.at(2).timeInTicks, 2);
    EXPECT_FLOAT_EQ(firstBoneFrames.positionFrames.at(0).position.x, 0.0f);
    EXPECT_FLOAT_EQ(firstBoneFrames.positionFrames.at(0).position.y, 0.0f);
    EXPECT_FLOAT_EQ(firstBoneFrames.positionFrames.at(0).position.z, 0.0f);
    EXPECT_FLOAT_EQ(firstBoneFrames.rotationFrames.at(1).rotation.x, 1.1f);
    EXPECT_FLOAT_EQ(firstBoneFrames.rotationFrames.at(1).rotation.y, 1.1f);
    EXPECT_FLOAT_EQ(firstBoneFrames.rotationFrames.at(1).rotation.z, 1.1f);
    EXPECT_FLOAT_EQ(firstBoneFrames.rotationFrames.at(1).rotation.w, 1.0f);
    EXPECT_FLOAT_EQ(firstBoneFrames.scaleFrames.at(2).scale.x, 2.2f);
    EXPECT_FLOAT_EQ(firstBoneFrames.scaleFrames.at(2).scale.y, 2.2f);
    EXPECT_FLOAT_EQ(firstBoneFrames.scaleFrames.at(2).scale.z, 2.2f);

    const auto& secondBoneFrames = actualAsset.framesPerBone.at("Bone1");
    EXPECT_EQ(secondBoneFrames.positionFrames.at(0).timeInTicks, 0);
    EXPECT_EQ(secondBoneFrames.positionFrames.at(1).timeInTicks, 1);
    EXPECT_EQ(secondBoneFrames.positionFrames.at(2).timeInTicks, 2);
    EXPECT_FLOAT_EQ(secondBoneFrames.positionFrames.at(0).position.x, 3.0f);
    EXPECT_FLOAT_EQ(secondBoneFrames.positionFrames.at(0).position.y, 3.0f);
    EXPECT_FLOAT_EQ(secondBoneFrames.positionFrames.at(0).position.z, 3.0f);
    EXPECT_FLOAT_EQ(secondBoneFrames.rotationFrames.at(1).rotation.x, 4.1f);
    EXPECT_FLOAT_EQ(secondBoneFrames.rotationFrames.at(1).rotation.y, 4.1f);
    EXPECT_FLOAT_EQ(secondBoneFrames.rotationFrames.at(1).rotation.z, 4.1f);
    EXPECT_FLOAT_EQ(secondBoneFrames.rotationFrames.at(1).rotation.w, 4.0f);
    EXPECT_FLOAT_EQ(secondBoneFrames.scaleFrames.at(2).scale.x, 5.2f);
    EXPECT_FLOAT_EQ(secondBoneFrames.scaleFrames.at(2).scale.y, 5.2f);
    EXPECT_FLOAT_EQ(secondBoneFrames.scaleFrames.at(2).scale.z, 5.2f);
}

TEST(AssetSerializationTest, DeserializeAudioClip_unsupportedVersion_throws)
{
    const auto dummyAsset = nc::asset::AudioClip{};
    const auto unsupportedVersion = 1ull;
    auto stream = std::stringstream{std::ios::in | std::ios::out | std::ios::binary};
    nc::convert::Serialize(stream, dummyAsset, unsupportedVersion);
    EXPECT_THROW(nc::asset::DeserializeAudioClip(stream), nc::NcError);
}

TEST(AssetSerializationTest, DeserializeConcaveCollider_unsupportedVersion_throws)
{
    const auto dummyAsset = nc::asset::ConcaveCollider{};
    const auto unsupportedVersion = 1ull;
    auto stream = std::stringstream{std::ios::in | std::ios::out | std::ios::binary};
    nc::convert::Serialize(stream, dummyAsset, unsupportedVersion);
    EXPECT_THROW(nc::asset::DeserializeConcaveCollider(stream), nc::NcError);
}

TEST(AssetSerializationTest, DeserializeCubeMap_unsupportedVersion_throws)
{
    const auto dummyAsset = nc::asset::CubeMap{};
    const auto unsupportedVersion = 1ull;
    auto stream = std::stringstream{std::ios::in | std::ios::out | std::ios::binary};
    nc::convert::Serialize(stream, dummyAsset, unsupportedVersion);
    EXPECT_THROW(nc::asset::DeserializeCubeMap(stream), nc::NcError);
}

TEST(AssetSerializationTest, DeserializeHullCollider_unsupportedVersion_throws)
{
    const auto dummyAsset = nc::asset::HullCollider{};
    const auto unsupportedVersion = 1ull;
    auto stream = std::stringstream{std::ios::in | std::ios::out | std::ios::binary};
    nc::convert::Serialize(stream, dummyAsset, unsupportedVersion);
    EXPECT_THROW(nc::asset::DeserializeHullCollider(stream), nc::NcError);
}

TEST(AssetSerializationTest, DeserializeMesh_unsupportedVersion_throws)
{
    const auto dummyAsset = nc::asset::Mesh{};
    const auto unsupportedVersion = 1ull;
    auto stream = std::stringstream{std::ios::in | std::ios::out | std::ios::binary};
    nc::convert::Serialize(stream, dummyAsset, unsupportedVersion);
    EXPECT_THROW(nc::asset::DeserializeMesh(stream), nc::NcError);
}

TEST(AssetSerializationTest, DeserializeSkeletalAnimation_unsupportedVersion_throws)
{
    const auto dummyAsset = nc::asset::SkeletalAnimation{};
    const auto unsupportedVersion = 1ull;
    auto stream = std::stringstream{std::ios::in | std::ios::out | std::ios::binary};
    nc::convert::Serialize(stream, dummyAsset, unsupportedVersion);
    EXPECT_THROW(nc::asset::DeserializeSkeletalAnimation(stream), nc::NcError);
}

TEST(AssetSerializationTest, DeserializeTexture_unsupportedVersion_throws)
{
    const auto dummyAsset = nc::asset::Texture{};
    const auto unsupportedVersion = 1ull;
    auto stream = std::stringstream{std::ios::in | std::ios::out | std::ios::binary};
    nc::convert::Serialize(stream, dummyAsset, unsupportedVersion);
    EXPECT_THROW(nc::asset::DeserializeTexture(stream), nc::NcError);
}
