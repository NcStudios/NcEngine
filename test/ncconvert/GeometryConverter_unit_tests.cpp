#include "gtest/gtest.h"
#include "GeometryTestUtility.h"
#include "CollateralGeometry.h"

#include "analysis/GeometryAnalysis.h"
#include "converters/GeometryConverter.h"
#include "DirectXMath.h"
#include "ncasset/Assets.h"
#include "ncutility/NcError.h"

#include <algorithm>
#include <array>
#include <iostream>
#include <string>

TEST(GeometryConverterTest, ImportConcaveCollider_convertsToNca)
{
    namespace test_data = collateral::plane_fbx;
    auto uut = nc::convert::GeometryConverter{};
    const auto actual = uut.ImportConcaveCollider(test_data::filePath);

    EXPECT_EQ(actual.extents, test_data::meshVertexExtents);
    EXPECT_FLOAT_EQ(actual.maxExtent, test_data::furthestDistanceFromOrigin);
    EXPECT_EQ(actual.triangles.size(), test_data::triangleCount);

    for (const auto& tri : actual.triangles)
    {
        const auto pos = std::ranges::find(test_data::possibleTriangles, tri);
        EXPECT_NE(pos, test_data::possibleTriangles.cend());
    }
}

TEST(GeometryConverterTest, ImportedHullCollider_convertsToNca)
{
    namespace test_data = collateral::cube_fbx;
    auto uut = nc::convert::GeometryConverter{};
    const auto actual = uut.ImportHullCollider(test_data::filePath);

    EXPECT_EQ(actual.extents, test_data::meshVertexExtents);
    EXPECT_FLOAT_EQ(actual.maxExtent, test_data::furthestDistanceFromOrigin);
    EXPECT_EQ(actual.vertices.size(), test_data::vertexCount);

    for (const auto& vertex : actual.vertices)
    {
        const auto pos = std::ranges::find(test_data::possibleVertices, vertex);
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
        const auto tangentPos = std::ranges::find(test_data::possibleTangents, vertex.tangent);
        const auto bitangentPos = std::ranges::find(test_data::possibleBitangents, vertex.bitangent);
        EXPECT_NE(vertexPos, test_data::possibleVertices.cend());
        EXPECT_NE(normalPos, test_data::possibleNormals.cend());
        EXPECT_NE(tangentPos, test_data::possibleTangents.cend());
        EXPECT_NE(bitangentPos, test_data::possibleBitangents.cend());
    }

    const auto nVertices = actual.vertices.size();
    EXPECT_TRUE(std::ranges::all_of(actual.indices, [&nVertices](auto i){ return i < nVertices; }));
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
    catch(const nc::NcError& e)
    {
        EXPECT_TRUE(std::string(e.what()).find(std::string("more than four bones")) != std::string::npos);
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
    catch(const nc::NcError& e)
    {
        EXPECT_TRUE(std::string(e.what()).find(std::string("affecting each vertex must equal 1")) != std::string::npos);
        threwNcError = true;
    }
    
    EXPECT_TRUE(threwNcError);
}

TEST(GeometryConverterTest, GetBonesData_rootBoneOffset_equalsGlobalInverse)
{
    namespace test_data = collateral::single_bone_four_vertex_fbx;
    auto uut = nc::convert::GeometryConverter{};
    const auto bonesData = uut.ImportMesh(test_data::filePath).bonesData.value();

    DirectX::XMFLOAT4X4 view;
    XMStoreFloat4x4(&view, bonesData.vertexSpaceToBoneSpace[0].transformationMatrix);
 
    float a1 = view._11;
    float a2 = view._12;
    float a3 = view._13;
    float a4 = view._14;
 
    float b1 = view._21;
    float b2 = view._22;
    float b3 = view._23;
    float b4 = view._24;
 
    float c1 = view._31;
    float c2 = view._32;
    float c3 = view._33;
    float c4 = view._34;
 
    float d1 = view._41;
    float d2 = view._42;
    float d3 = view._43;
    float d4 = view._44;
 
    EXPECT_EQ(a1, 1);
    EXPECT_EQ(a2, 0);
    EXPECT_EQ(a3, 0);
    EXPECT_EQ(a4, 0);
     
    EXPECT_EQ(b1, 0);
    EXPECT_EQ(b2, 0);
    EXPECT_EQ(b3, 1);
    EXPECT_EQ(b4, 0);

    EXPECT_EQ(c1, 0);
    EXPECT_EQ(c2, -1);
    EXPECT_EQ(c3, 0);
    EXPECT_EQ(c4, 0);
    
    EXPECT_EQ(d1, 0);
    EXPECT_EQ(d2, 0);
    EXPECT_EQ(d3, 0);
    EXPECT_EQ(d4, 1);
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
        EXPECT_EQ(vertex.boneIds[0], 0); // Bone0
        EXPECT_EQ(vertex.boneIds[1], 1); // Bone1
        EXPECT_EQ(vertex.boneIds[2], 2); // Bone2
        EXPECT_EQ(vertex.boneIds[3], 3); // Bone3
        EXPECT_FLOAT_EQ(vertex.boneWeights.x, 0.1f);  // Bone0
        EXPECT_FLOAT_EQ(vertex.boneWeights.y, 0.1f);  // Bone1
        EXPECT_FLOAT_EQ(vertex.boneWeights.z, 0.1f);  // Bone2
        EXPECT_FLOAT_EQ(vertex.boneWeights.w, 0.7f);  // Bone3
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

    EXPECT_FLOAT_EQ(actual.vertices[0].boneWeights.x, 0.35232919f);  // Bone0
    EXPECT_FLOAT_EQ(actual.vertices[0].boneWeights.y, 0.17152755f);  // Bone1
    EXPECT_FLOAT_EQ(actual.vertices[0].boneWeights.z, 0.11047833f);  // Bone2
    EXPECT_FLOAT_EQ(actual.vertices[0].boneWeights.w, 0.36566496f);  // Bone3
    
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
    const auto actual = uut.ImportSkeletalAnimation(test_data::filePath, std::string("Armature|Wiggle"));

    EXPECT_EQ(actual.name, std::string("Armature|Wiggle"));
    EXPECT_EQ(actual.durationInTicks, 60);
    EXPECT_EQ(actual.ticksPerSecond, 24);
    EXPECT_EQ(actual.framesPerBone.size(), 4);
}

TEST(GeometryConverterTest, ImportSkeletalAnimation_incorrectSubResourceName_throws)
{
    namespace test_data = collateral::simple_cube_animation_fbx;
    auto uut = nc::convert::GeometryConverter{};
    EXPECT_THROW(uut.ImportSkeletalAnimation(test_data::filePath, std::string("Armature|Wigglde")), nc::NcError);
}
