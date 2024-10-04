#include "gtest/gtest.h"
#include "CollateralAudio.h"
#include "CollateralGeometry.h"
#include "CollateralTexture.h"
#include "GeometryTestUtility.h"
#include "TextureTestUtility.h"

#include "ncasset/Assets.h"
#include "ncasset/AssetType.h"
#include "ncasset/Import.h"
#include "ncconvert/builder/Builder.h"
#include "ncconvert/builder/Target.h"
#include "ncconvert/converters/GeometryConverter.h"
#include "ncconvert/converters/TextureConverter.h"

const auto ncaTestOutDirectory = std::filesystem::path{"./test_temp_dir"};

class BuildAndImportTest : public ::testing::Test
{
    public:
        static void SetUpTestSuite()
        {
            if (!std::filesystem::exists(ncaTestOutDirectory))
            {
                std::filesystem::create_directory(ncaTestOutDirectory);
            }
        }

        static void TearDownTestSuite()
        {
            if (std::filesystem::exists(ncaTestOutDirectory))
            {
                std::filesystem::remove_all(ncaTestOutDirectory);
            }
        }
};

TEST_F(BuildAndImportTest, Texture_from_png)
{
    namespace test_data = collateral::rgb_corners;
    const auto inFile = test_data::pngFilePath;
    const auto outFile = ncaTestOutDirectory / "rgb_png.nca";
    const auto target = nc::convert::Target{inFile, outFile};
    auto builder = nc::convert::Builder{};
    ASSERT_TRUE(builder.Build(nc::asset::AssetType::Texture, target));

    const auto asset = nc::asset::ImportTexture(outFile);

    EXPECT_EQ(test_data::width, asset.width);
    EXPECT_EQ(test_data::height, asset.height);
    ASSERT_EQ(test_data::numBytes, asset.pixelData.size());

    for (auto pixelIndex = 0u; pixelIndex < test_data::numPixels; ++pixelIndex)
    {
        const auto expectedPixel = test_data::pixels[pixelIndex];
        const auto actualPixel = ReadPixel(asset.pixelData.data(), pixelIndex * 4);
        EXPECT_EQ(expectedPixel, actualPixel);
    }
}

TEST_F(BuildAndImportTest, ConcaveCollider_from_fbx)
{
    namespace test_data = collateral::plane_fbx;
    const auto inFile = test_data::filePath;
    const auto outFile = ncaTestOutDirectory / "plane_concave.nca";
    const auto target = nc::convert::Target{inFile, outFile};
    auto builder = nc::convert::Builder{};
    ASSERT_TRUE(builder.Build(nc::asset::AssetType::ConcaveCollider, target));

    const auto asset = nc::asset::ImportConcaveCollider(outFile);

    EXPECT_EQ(asset.extents, test_data::meshVertexExtents);
    EXPECT_FLOAT_EQ(asset.maxExtent, test_data::furthestDistanceFromOrigin);
    EXPECT_EQ(asset.triangles.size(), test_data::triangleCount);

    for (const auto& tri : asset.triangles)
    {
        const auto pos = std::ranges::find(test_data::possibleTriangles, tri);
        EXPECT_NE(pos, test_data::possibleTriangles.cend());
    }
}

TEST_F(BuildAndImportTest, HullCollider_from_fbx)
{
    namespace test_data = collateral::cube_fbx;
    const auto inFile = test_data::filePath;
    const auto outFile = ncaTestOutDirectory / "cube_hull.nca";
    const auto target = nc::convert::Target{inFile, outFile};
    auto builder = nc::convert::Builder{};
    ASSERT_TRUE(builder.Build(nc::asset::AssetType::HullCollider, target));

    const auto asset = nc::asset::ImportHullCollider(outFile);

    EXPECT_EQ(asset.extents, test_data::meshVertexExtents);
    EXPECT_FLOAT_EQ(asset.maxExtent, test_data::furthestDistanceFromOrigin);
    EXPECT_EQ(asset.vertices.size(), test_data::vertexCount);

    for (const auto& vertex : asset.vertices)
    {
        const auto pos = std::ranges::find(test_data::possibleVertices, vertex);
        EXPECT_NE(pos, test_data::possibleVertices.cend());
    }
}

TEST_F(BuildAndImportTest, Mesh_from_fbx)
{
    namespace test_data = collateral::cube_fbx;
    const auto inFile = test_data::filePath;
    const auto outFile = ncaTestOutDirectory / "cube_mesh.nca";
    const auto target = nc::convert::Target{inFile, outFile};
    auto builder = nc::convert::Builder{};
    ASSERT_TRUE(builder.Build(nc::asset::AssetType::Mesh, target));

    const auto asset = nc::asset::ImportMesh(outFile);

    EXPECT_EQ(asset.extents, test_data::meshVertexExtents);
    EXPECT_FLOAT_EQ(asset.maxExtent, test_data::furthestDistanceFromOrigin);
    EXPECT_EQ(asset.vertices.size(), test_data::vertexCount);

    for (const auto& vertex : asset.vertices)
    {
        // TODO: uvs
        const auto foundVertex = std::ranges::find(test_data::possibleVertices, vertex.position);
        const auto foundNormal = std::ranges::find(test_data::possibleNormals, vertex.normal);
        const auto foundTangent = std::ranges::find(test_data::possibleTangents, vertex.tangent);
        const auto foundBitangent = std::ranges::find(test_data::possibleBitangents, vertex.bitangent);
        EXPECT_NE(foundVertex, test_data::possibleVertices.cend());
        EXPECT_NE(foundNormal, test_data::possibleNormals.cend());
        EXPECT_NE(foundTangent, test_data::possibleTangents.cend());
        EXPECT_NE(foundBitangent, test_data::possibleBitangents.cend());
    }

    // should have triangular faces
    EXPECT_EQ(asset.indices.size() % 3, 0);

    // just verifying all indices point to a valid vertex
    const auto nVertices = asset.vertices.size();
    EXPECT_TRUE(std::ranges::all_of(asset.indices, [&nVertices](auto i){ return i < nVertices; }));
}

TEST_F(BuildAndImportTest, SkeletalAnimation_from_fbx)
{
    namespace test_data = collateral::simple_cube_animation_fbx;
    const auto inFile = test_data::filePath;
    const auto outFile = ncaTestOutDirectory / "simple_cube_animation.nca";
    const auto target = nc::convert::Target(inFile, outFile, std::string{"Armature|Wiggle"});
    auto builder = nc::convert::Builder{};
    ASSERT_TRUE(builder.Build(nc::asset::AssetType::SkeletalAnimation, target));

    auto asset = nc::asset::ImportSkeletalAnimation(outFile);

    EXPECT_EQ(asset.name, "Armature|Wiggle");
    EXPECT_EQ(asset.durationInTicks, 60);
    EXPECT_EQ(asset.ticksPerSecond, 24);
    EXPECT_EQ(asset.framesPerBone.size(), 4);

    EXPECT_EQ(asset.framesPerBone["Armature"].positionFrames.size(), 2);
    EXPECT_EQ(asset.framesPerBone["Armature"].rotationFrames.size(), 2);
    EXPECT_EQ(asset.framesPerBone["Armature"].rotationFrames.size(), 2);

    EXPECT_EQ(asset.framesPerBone["Root"].positionFrames.size(), 60);
    EXPECT_EQ(asset.framesPerBone["Root"].rotationFrames.size(), 60);
    EXPECT_EQ(asset.framesPerBone["Root"].rotationFrames.size(), 60);
    
    EXPECT_EQ(asset.framesPerBone["Tip"].positionFrames.size(), 2);
    EXPECT_EQ(asset.framesPerBone["Tip"].rotationFrames.size(), 2);
    EXPECT_EQ(asset.framesPerBone["Tip"].rotationFrames.size(), 2);

    EXPECT_EQ(asset.framesPerBone["Tip_end"].positionFrames.size(), 61);
    EXPECT_EQ(asset.framesPerBone["Tip_end"].rotationFrames.size(), 61);
    EXPECT_EQ(asset.framesPerBone["Tip_end"].rotationFrames.size(), 61);
}

TEST_F(BuildAndImportTest, AudioClip_from_wav)
{
    namespace test_data = collateral::sine;
    const auto inFile = test_data::filePath;
    const auto outFile = ncaTestOutDirectory / "sine.nca";
    const auto target = nc::convert::Target{inFile, outFile};
    auto builder = nc::convert::Builder{};
    ASSERT_TRUE(builder.Build(nc::asset::AssetType::AudioClip, target));

    const auto asset = nc::asset::ImportAudioClip(outFile);

    EXPECT_EQ(asset.samplesPerChannel, test_data::samplesPerChannel);
    ASSERT_EQ(asset.leftChannel.size(), test_data::leftChannel.size());
    ASSERT_EQ(asset.rightChannel.size(), test_data::rightChannel.size());
    EXPECT_TRUE(std::equal(asset.leftChannel.cbegin(),
                           asset.leftChannel.cend(),
                           test_data::leftChannel.cbegin()));
    EXPECT_TRUE(std::equal(asset.rightChannel.cbegin(),
                           asset.rightChannel.cend(),
                           test_data::rightChannel.cbegin()));
}

TEST_F(BuildAndImportTest, CubeMap_from_png)
{
    namespace test_data = collateral::cube_map;
    const auto inFile = test_data::horizontalCrossFilePath;
    const auto outFile = ncaTestOutDirectory / "cube_map.nca";
    const auto target = nc::convert::Target{inFile, outFile};
    auto builder = nc::convert::Builder{};
    ASSERT_TRUE(builder.Build(nc::asset::AssetType::CubeMap, target));

    const auto asset = nc::asset::ImportCubeMap(outFile);

    EXPECT_EQ(asset.faceSideLength, test_data::faceSideLength);
    ASSERT_EQ(asset.pixelData.size(), test_data::numBytes);

    for (auto pixelIndex = 0u; pixelIndex < test_data::numPixels; ++pixelIndex)
    {
        const auto expectedPixel = test_data::pixels[pixelIndex];
        const auto actualPixel = ReadPixel(asset.pixelData.data(), pixelIndex * 4);
        EXPECT_EQ(expectedPixel, actualPixel);
    }
}
