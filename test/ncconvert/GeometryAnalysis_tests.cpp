#include "gtest/gtest.h"
#include "analysis/GeometryAnalysis.h"

#include "ncasset/Assets.h"

#include <array>

namespace test_input_1
{
using namespace nc;
constexpr auto extents = Vector3{1.3f, 2.0f, 1.3f};
const auto maxDistance = Magnitude(Vector3::One());

constexpr auto vectors = std::array<Vector3, 6>{
    Vector3::Up(), Vector3::Down(), Vector3::Zero(),
    Vector3::One(), Vector3::Splat(0.5f), Vector3::Splat(-0.3f)
};

constexpr auto triangles = std::array<Triangle, 2>{
    Triangle{vectors[0], vectors[1], vectors[2]},
    Triangle{vectors[3], vectors[4], vectors[5]}
};

constexpr auto meshVertices = std::array<asset::MeshVertex, 6>{
    asset::MeshVertex{vectors[0]}, asset::MeshVertex{vectors[1]},
    asset::MeshVertex{vectors[2]}, asset::MeshVertex{vectors[3]},
    asset::MeshVertex{vectors[3]}, asset::MeshVertex{vectors[5]}
};
} // namespace test_input_1

TEST(GeometryAnalysisTest, FindFurthestDistanceFromOrigin_Vector3)
{
    const auto actual = nc::convert::FindFurthestDistanceFromOrigin(test_input_1::vectors);
    EXPECT_FLOAT_EQ(test_input_1::maxDistance, actual);
}

TEST(GeometryAnalysisTest, FindFurthestDistanceFromOrigin_Triangle)
{
    const auto actual = nc::convert::FindFurthestDistanceFromOrigin(test_input_1::triangles);
    EXPECT_FLOAT_EQ(test_input_1::maxDistance, actual);
}

TEST(GeometryAnalysisTest, FindFurthestDistanceFromOrigin_MeshVertex)
{
    const auto actual = nc::convert::FindFurthestDistanceFromOrigin(test_input_1::meshVertices);
    EXPECT_FLOAT_EQ(test_input_1::maxDistance, actual);
}

TEST(GeometryAnalysisTest, GetMeshVertexExtents_Vector3)
{
    const auto actual = nc::convert::GetMeshVertexExtents(test_input_1::vectors);
    EXPECT_EQ(test_input_1::extents, actual);
}

TEST(GeometryAnalysisTest, GetMeshVertexExtents_Triangle)
{
    const auto actual = nc::convert::GetMeshVertexExtents(test_input_1::triangles);
    EXPECT_EQ(test_input_1::extents, actual);
}

TEST(GeometryAnalysisTest, GetMeshVertexExtents_MeshVertex)
{
    const auto actual = nc::convert::GetMeshVertexExtents(test_input_1::meshVertices);
    EXPECT_EQ(test_input_1::extents, actual);
}
