#pragma once

#include <span>

namespace nc
{
struct Triangle;
struct Vector3;

namespace asset
{
struct MeshVertex;
} // namespace asset

namespace convert
{
auto FindFurthestDistanceFromOrigin(std::span<const Vector3> data) -> float;
auto FindFurthestDistanceFromOrigin(std::span<const asset::MeshVertex> data) -> float;
auto FindFurthestDistanceFromOrigin(std::span<const Triangle> data) -> float;
auto GetMeshVertexExtents(std::span<const Vector3> data) -> Vector3;
auto GetMeshVertexExtents(std::span<const asset::MeshVertex> data) -> Vector3;
auto GetMeshVertexExtents(std::span<const Triangle> data) -> Vector3;
} // namespace convert
} // namespace nc
