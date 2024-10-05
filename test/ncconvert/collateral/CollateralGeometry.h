#pragma once

#include "CollateralCommon.h"
#include "analysis/GeometryAnalysis.h"

#include "ncmath/Geometry.h"

#include <array>

namespace collateral
{

// Describes the collateral file cube.fbx
namespace cube_fbx
{
const auto filePath = collateralDirectory / "cube.fbx";
constexpr auto vertexCount = 24ull; // note that duplicates are removed below
constexpr auto possibleVertices = std::array<nc::Vector3, 8>{
    nc::Vector3{ 0.5f,  0.5f,  0.5f}, nc::Vector3{-0.5f,  0.5f,  0.5f},
    nc::Vector3{ 0.5f,  0.5f, -0.5f}, nc::Vector3{-0.5f,  0.5f, -0.5f},
    nc::Vector3{ 0.5f, -0.5f,  0.5f}, nc::Vector3{-0.5f, -0.5f,  0.5f},
    nc::Vector3{ 0.5f, -0.5f, -0.5f}, nc::Vector3{-0.5f, -0.5f, -0.5f}
};

constexpr auto possibleNormals = std::array<nc::Vector3, 6>{
    nc::Vector3::Left(), nc::Vector3::Right(), nc::Vector3::Up(),
    nc::Vector3::Down(), nc::Vector3::Front(), nc::Vector3::Back()
};

constexpr auto possibleTangents = std::array<nc::Vector3, 3>{
    nc::Vector3::Left(), nc::Vector3::Right(), nc::Vector3::Back()
};

constexpr auto possibleBitangents = std::array<nc::Vector3, 4>{
    nc::Vector3::Left(), nc::Vector3::Right(), nc::Vector3::Up(),
    nc::Vector3::Down()
};

constexpr auto meshVertexExtents = nc::Vector3::One();
const auto furthestDistanceFromOrigin = std::sqrt(0.75f);
} // namespace cube_fbx

// Describes the collateral file single_bone_four_vertex.fbx
namespace single_bone_four_vertex_fbx
{
const auto filePath = collateralDirectory / "single_bone_four_vertex.fbx";
} // namespace single_bone_four_vertex_fbx

// Describes the collateral file four_bone_four_vertex.fbx
namespace four_bone_four_vertex_fbx
{
const auto filePath = collateralDirectory / "four_bone_four_vertex.fbx";
} // namespace four_bone_four_vertex_fbx

// Describes the collateral file five_bones_per_vertex.fbx
namespace five_bones_per_vertex_fbx
{
const auto filePath = collateralDirectory / "five_bones_per_vertex.fbx";
} // namespace five_bones_per_vertex_fbx

// Describes the collateral file four_bones_neq100.fbx
namespace four_bones_neq100_fbx
{
const auto filePath = collateralDirectory / "four_bones_neq100.fbx";
} // namespace four_bones_neq100_fbx

// Describes the collateral file four_bones_one_bone_70_percent.fbx
namespace four_bones_one_bone_70_percent_fbx
{
const auto filePath = collateralDirectory / "four_bones_one_bone_70_percent.fbx";
} // namespace four_bones_one_bone_70_percent_fbx

// Describes the collateral file real_world_model.fbx
namespace real_world_model_fbx
{
const auto filePath = collateralDirectory / "real_world_model.fbx";
} // namespace real_world_model_fbx

// Describes the collateral file simple_cube_animation.fbx
namespace simple_cube_animation_fbx
{
const auto filePath = collateralDirectory / "simple_cube_animation.fbx";
} // namespace simple_cube_animation_fbx

// Describes the collateral file multicube.fbx
namespace multicube_fbx
{
const auto filePath = collateralDirectory / "multicube.fbx";
} // namespace multicube_fbx

// Describes the collateral file plane_and_cube.fbx
namespace plane_and_cube_fbx
{
const auto filePath = collateralDirectory / "plane_and_cube.fbx";
} // namespace plane_and_cube_fbx

// Describes the collateral file plane.fbx
namespace plane_fbx
{
const auto filePath = collateralDirectory / "plane.fbx";
constexpr auto triangleCount = 2ull;
constexpr auto possibleTriangles = std::array<nc::Triangle, triangleCount>{
    nc::Triangle{
        nc::Vector3{-1.0f, -1.0f, 0.0f},
        nc::Vector3{-1.0f,  1.0f, 0.0f},
        nc::Vector3{ 1.0f, -1.0f, 0.0f}
    },
    nc::Triangle{
        nc::Vector3{-1.0f,  1.0f, 0.0f},
        nc::Vector3{ 1.0f,  1.0f, 0.0f},
        nc::Vector3{ 1.0f, -1.0f, 0.0f}
    }
};

constexpr auto meshVertexExtents = nc::Vector3{2.0f, 2.0f, 0.0f};
const auto furthestDistanceFromOrigin = std::sqrt(2.0f);
} // namespace plane_fbx
} // namespace collateral
