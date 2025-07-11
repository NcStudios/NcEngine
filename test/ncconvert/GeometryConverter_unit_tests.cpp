#include "CollateralGeometry.h"
#include "GeometryTestUtility.h"
#include "gtest/gtest.h"

#include "analysis/GeometryAnalysis.h"
#include "converters/GeometryConverter.h"
#include "DirectXMath.h"
#include "ncasset/Assets.h"
#include "ncutility/NcError.h"

#include <algorithm>
#include <array>
#include <iostream>
#include <string>

TEST(GeometryConverterTest, ImportMeshCollider_convertsToNca)
{
    namespace test_data = collateral::plane_fbx;
    auto uut = nc::convert::GeometryConverter{};
    const auto actual = uut.ImportMeshCollider(test_data::filePath);

    EXPECT_EQ(actual.extents, test_data::meshVertexExtents);
    EXPECT_FLOAT_EQ(actual.maxExtent, test_data::furthestDistanceFromOrigin);
    EXPECT_FALSE(actual.blob.empty());

    auto reconstituted = nc::jolt::DeserializeShape(actual.blob);
    auto actualMesh = UpcastToMeshShape(reconstituted.GetPtr());
    const auto actualTriangles = GetTriangles(*actualMesh, test_data::triangleCount);
    ASSERT_EQ(test_data::triangleCount, actualTriangles.size());

    for (const auto& tri : actualTriangles)
    {
        const auto pos = std::ranges::find(test_data::possibleTriangles, tri);
        EXPECT_NE(pos, test_data::possibleTriangles.cend());
    }
}

TEST(GeometryConverterTest, ImportedConvexHull_convertsToNca)
{
    namespace test_data = collateral::cube_fbx;
    auto uut = nc::convert::GeometryConverter{};
    const auto actual = uut.ImportConvexHull(test_data::filePath);

    EXPECT_EQ(actual.extents, test_data::meshVertexExtents);
    EXPECT_FLOAT_EQ(actual.maxExtent, test_data::furthestDistanceFromOrigin);

    auto reconstituted = nc::jolt::DeserializeShape(actual.blob);
    auto actualHull = UpcastToConvexHull(reconstituted.GetPtr());
    EXPECT_EQ(test_data::possibleVertices.size(), actualHull->GetNumPoints());
    for (auto i = 0u; i < actualHull->GetNumPoints(); ++i)
    {
        const auto point = actualHull->GetPoint(i);
        const auto pos = std::ranges::find(test_data::possibleVertices, ToVector3(point));
        EXPECT_NE(pos, test_data::possibleVertices.cend());
    }
}

TEST(GeometryConverterTest, ImportedMesh_convertsToNca)
{
    namespace test_data = collateral::cube_fbx;
    auto uut = nc::convert::GeometryConverter{};
    const auto actual = uut.ImportMesh(test_data::filePath);

    EXPECT_EQ(actual.extents, test_data::meshVertexExtents);
    EXPECT_FLOAT_EQ(actual.maxExtent, test_data::furthestDistanceFromOrigin);
    EXPECT_EQ(actual.vertices.size(), test_data::vertexCount);

    for (const auto& vertex : actual.vertices)
    {
        // TODO: uvs
        const auto vertexPos = std::ranges::find(test_data::possibleVertices, vertex.position);
        const auto normalPos = std::ranges::find(test_data::possibleNormals, vertex.normal);
        EXPECT_NE(vertexPos, test_data::possibleVertices.cend());
        EXPECT_NE(normalPos, test_data::possibleNormals.cend());
    }

    const auto nVertices = actual.vertices.size();
    EXPECT_TRUE(
        std::ranges::all_of(actual.indices, [&nVertices](auto i) { return i < nVertices; }));
}

TEST(GeometryConverterTest, ImportedMesh_optimizeMesh_convertsToNca)
{
    namespace test_data = collateral::cube_fbx;
    auto uut = nc::convert::GeometryConverter{};
    const auto actual = uut.ImportMesh(test_data::filePath, std::nullopt, true);

    EXPECT_EQ(actual.extents, test_data::meshVertexExtents);
    EXPECT_FLOAT_EQ(actual.maxExtent, test_data::furthestDistanceFromOrigin);
    EXPECT_EQ(actual.vertices.size(), test_data::vertexCount);

    for (const auto& vertex : actual.vertices)
    {
        const auto vertexPos = std::ranges::find(test_data::possibleVertices, vertex.position);
        const auto normalPos = std::ranges::find(test_data::possibleNormals, vertex.normal);
        EXPECT_NE(vertexPos, test_data::possibleVertices.cend());
        EXPECT_NE(normalPos, test_data::possibleNormals.cend());
    }

    const auto nVertices = actual.vertices.size();
    EXPECT_TRUE(
        std::ranges::all_of(actual.indices, [&nVertices](auto i) { return i < nVertices; }));
}

TEST(GeometryConverterTest, ImportedMesh_multipleSubResources_specifiedMeshParsed)
{
    namespace test_data = collateral::plane_and_cube_fbx;
    auto uut = nc::convert::GeometryConverter{};
    const auto cubeMesh = uut.ImportMesh(test_data::filePath, std::string{"Cube Mesh"});
    const auto planeMesh = uut.ImportMesh(test_data::filePath, std::string{"Plane Mesh"});

    EXPECT_NE(cubeMesh.vertices.size(), planeMesh.vertices.size());
    EXPECT_EQ(cubeMesh.vertices.size(), 24);
    EXPECT_EQ(planeMesh.vertices.size(), 4);
}

TEST(GeometryConverterTest, GetBoneWeights_singleBone_1WeightAllVertices)
{
    namespace test_data = collateral::single_bone_four_vertex_fbx;
    auto uut = nc::convert::GeometryConverter{};
    const auto actual = uut.ImportMesh(test_data::filePath);

    for (const auto& vertex : actual.vertices)
    {
        EXPECT_EQ(vertex.boneIds[0], 0);
        EXPECT_EQ(vertex.boneIds[1], 0);
        EXPECT_EQ(vertex.boneIds[2], 0);
        EXPECT_EQ(vertex.boneIds[3], 0);
        EXPECT_EQ(vertex.boneWeights.x, 1);
        EXPECT_EQ(vertex.boneWeights.y, -1);
        EXPECT_EQ(vertex.boneWeights.z, -1);
        EXPECT_EQ(vertex.boneWeights.w, -1);
    }
}

TEST(GeometryConverterTest, GetBoneWeights_fourBones_quarterWeightAllVertices)
{
    namespace test_data = collateral::four_bone_four_vertex_fbx;
    auto uut = nc::convert::GeometryConverter{};
    const auto actual = uut.ImportMesh(test_data::filePath);

    for (const auto& vertex : actual.vertices)
    {
        EXPECT_EQ(vertex.boneIds[0], 0);
        EXPECT_EQ(vertex.boneIds[1], 1);
        EXPECT_EQ(vertex.boneIds[2], 2);
        EXPECT_EQ(vertex.boneIds[3], 3);
        EXPECT_EQ(vertex.boneWeights.x, 0.25);
        EXPECT_EQ(vertex.boneWeights.y, 0.25);
        EXPECT_EQ(vertex.boneWeights.z, 0.25);
        EXPECT_EQ(vertex.boneWeights.w, 0.25);
    }
}

TEST(GeometryConverterTest, GetBoneWeights_fiveBonesPerVertex_importFails)
{
    namespace test_data = collateral::five_bones_per_vertex_fbx;
    auto uut = nc::convert::GeometryConverter{};
    auto threwNcError = false;
    try
    {
        uut.ImportMesh(test_data::filePath);
    }
    catch (const nc::NcError& e)
    {
        EXPECT_TRUE(std::string(e.what()).find(std::string("more than four bones")) !=
                    std::string::npos);
        threwNcError = true;
    }

    EXPECT_TRUE(threwNcError);
}

TEST(GeometryConverterTest, GetBoneWeights_weightsNotEqual100_importFails)
{
    namespace test_data = collateral::four_bones_neq100_fbx;
    auto uut = nc::convert::GeometryConverter{};
    auto threwNcError = false;
    try
    {
        uut.ImportMesh(test_data::filePath);
    }
    catch (const nc::NcError& e)
    {
        EXPECT_TRUE(std::string(e.what()).find(std::string("affecting each vertex must equal 1")) !=
                    std::string::npos);
        threwNcError = true;
    }

    EXPECT_TRUE(threwNcError);
}

TEST(GeometryConverterTest, GetBonesData_matrixVectorsPopulated)
{
    namespace test_data = collateral::single_bone_four_vertex_fbx;
    auto uut = nc::convert::GeometryConverter{};
    const auto bonesData = uut.ImportMesh(test_data::filePath).bonesData.value();
    EXPECT_EQ(bonesData.boneSpaceToParentSpace.size(), 6);
    EXPECT_EQ(bonesData.vertexSpaceToBoneSpace.size(), 1);
    EXPECT_EQ(bonesData.boneSpaceToParentSpace[4].boneName, "Bone");
    EXPECT_EQ(bonesData.vertexSpaceToBoneSpace[0].boneName, "Bone");
}

TEST(GeometryConverterTest, GetBonesData_getBonesWeight_elementsCorrespond)
{
    namespace test_data = collateral::four_bones_one_bone_70_percent_fbx;
    auto uut = nc::convert::GeometryConverter{};
    const auto actual = uut.ImportMesh(test_data::filePath);

    for (const auto& vertex : actual.vertices)
    {
        EXPECT_EQ(vertex.boneIds[0], 0);             // Bone0
        EXPECT_EQ(vertex.boneIds[1], 1);             // Bone1
        EXPECT_EQ(vertex.boneIds[2], 2);             // Bone2
        EXPECT_EQ(vertex.boneIds[3], 3);             // Bone3
        EXPECT_FLOAT_EQ(vertex.boneWeights.x, 0.1f); // Bone0
        EXPECT_FLOAT_EQ(vertex.boneWeights.y, 0.1f); // Bone1
        EXPECT_FLOAT_EQ(vertex.boneWeights.z, 0.1f); // Bone2
        EXPECT_FLOAT_EQ(vertex.boneWeights.w, 0.7f); // Bone3
    }

    const auto& bonesData = actual.bonesData.value();
    EXPECT_EQ(bonesData.boneSpaceToParentSpace.size(), 10);
    EXPECT_EQ(bonesData.vertexSpaceToBoneSpace.size(), 5); // Four bones + Bone_End bone
    EXPECT_EQ(bonesData.vertexSpaceToBoneSpace[0].boneName, "Bone0");
    EXPECT_EQ(bonesData.vertexSpaceToBoneSpace[1].boneName, "Bone1");
    EXPECT_EQ(bonesData.vertexSpaceToBoneSpace[2].boneName, "Bone2");
    EXPECT_EQ(bonesData.vertexSpaceToBoneSpace[3].boneName, "Bone3");
}

TEST(GeometryConverterTest, GetBonesData_complexMesh_convertedCorrectly)
{
    namespace test_data = collateral::real_world_model_fbx;
    auto uut = nc::convert::GeometryConverter{};
    const auto actual = uut.ImportMesh(test_data::filePath);

    EXPECT_FLOAT_EQ(actual.vertices[0].boneWeights.x, 0.35232919f); // Bone0
    EXPECT_FLOAT_EQ(actual.vertices[0].boneWeights.y, 0.17152755f); // Bone1
    EXPECT_FLOAT_EQ(actual.vertices[0].boneWeights.z, 0.11047833f); // Bone2
    EXPECT_FLOAT_EQ(actual.vertices[0].boneWeights.w, 0.36566496f); // Bone3

    const auto& bonesData = actual.bonesData.value();
    EXPECT_EQ(bonesData.boneSpaceToParentSpace.size(), 383);
    EXPECT_EQ(bonesData.vertexSpaceToBoneSpace.size(), 283);
    EXPECT_EQ(bonesData.vertexSpaceToBoneSpace[0].boneName, "root");
    EXPECT_EQ(bonesData.vertexSpaceToBoneSpace[1].boneName, "DEF-spine.004");
    EXPECT_EQ(bonesData.vertexSpaceToBoneSpace[2].boneName, "DEF-spine.005");
    EXPECT_EQ(bonesData.vertexSpaceToBoneSpace[3].boneName, "DEF-spine.006");
}

TEST(GeometryConverterTest, ImportSkeletalAnimation_singleClip_convertedCorrectly)
{
    namespace test_data = collateral::simple_cube_animation_fbx;
    auto uut = nc::convert::GeometryConverter{};
    const auto actual =
        uut.ImportSkeletalAnimation(test_data::filePath, std::string("Armature|Wiggle"));

    EXPECT_EQ(actual.name, std::string("Armature|Wiggle"));
    EXPECT_EQ(actual.durationInTicks, 60);
    EXPECT_EQ(actual.ticksPerSecond, 24);
    EXPECT_EQ(actual.framesPerBone.size(), 4);
}

TEST(GeometryConverterTest, ImportSkeletalAnimation_incorrectSubResourceName_throws)
{
    namespace test_data = collateral::simple_cube_animation_fbx;
    auto uut = nc::convert::GeometryConverter{};
    EXPECT_THROW(uut.ImportSkeletalAnimation(test_data::filePath, std::string("Armature|Wigglde")),
                 nc::NcError);
}

TEST(GeometryConverterTest, ImportShapeKeyAnimation_plane_convertedCorrectly)
{
    namespace test_data = collateral::plane_glb;
    auto uut = nc::convert::GeometryConverter{};
    const auto actual = uut.ImportShapeKeyAnimation(test_data::filePath, std::string{"Y2"});

    EXPECT_EQ(actual.numShapeKeys, test_data::shapeKeyCount);
    EXPECT_EQ(actual.animation.width, test_data::vertexCount * 3u);
    EXPECT_EQ(actual.animation.height, test_data::shapeKeyCount);
    EXPECT_NEAR(actual.durationInSeconds, test_data::animationDuration, 0.0001f);
    EXPECT_EQ(actual.animation.pixelData.size(),
              test_data::vertexCount * 3ull * test_data::shapeKeyCount);
}

TEST(GeometryConverterTest, ImportShapeKeyAnimation_steeple_convertedCorrectly)
{
    namespace test_data = collateral::steeple_glb;
    auto uut = nc::convert::GeometryConverter{};
    const auto actual = uut.ImportShapeKeyAnimation(test_data::filePath, std::string{"Chute"});

    EXPECT_EQ(actual.numShapeKeys, test_data::shapeKeyCount);
    EXPECT_EQ(actual.animation.width, test_data::vertexCount * 3u);
    EXPECT_EQ(actual.animation.height, test_data::shapeKeyCount);
    EXPECT_NEAR(actual.durationInSeconds, test_data::animationDuration, 0.0001f);
    EXPECT_EQ(actual.animation.pixelData.size(),
              test_data::vertexCount * 3ull * test_data::shapeKeyCount);
}
