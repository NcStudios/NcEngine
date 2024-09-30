#include "GeometryAnalysis.h"
#include "utility/Log.h"

#include "ncasset/Assets.h"
#include "ncutility/NcError.h"

#include <algorithm>
#include <concepts>
#include <type_traits>

namespace
{
auto CompareSquareMagnitudes(const nc::Vector3& lhs, const nc::Vector3& rhs)
{
    return nc::SquareMagnitude(lhs) < nc::SquareMagnitude(rhs);
}

// Views a MeshVertex as just its position. For use with std::ranges projections.
auto PositionProjection(const nc::asset::MeshVertex& vertex) -> const nc::Vector3&
{
    return vertex.position;
}

auto GetMaximumVertexInDirection(std::span<const nc::Vector3> data, const nc::Vector3& direction) -> nc::Vector3
{
    NC_ASSERT(!data.empty(), "Vertex data is empty");
    auto maxElement = std::ranges::max_element(data, [&direction](auto&& lhs, auto&& rhs)
    {
        return nc::Dot(lhs, direction) < nc::Dot(rhs, direction);
    });

    return *maxElement;
}

auto GetMaximumVertexInDirection(std::span<const nc::asset::MeshVertex> data, const nc::Vector3& direction) -> nc::Vector3
{
    NC_ASSERT(!data.empty(), "Vertex data is empty");
    auto directionProjection = [&direction](auto&& lhs, auto&& rhs)
    {
        return nc::Dot(lhs, direction) < nc::Dot(rhs, direction);
    };

    auto maxElement = std::ranges::max_element(data, directionProjection, &PositionProjection);
    return maxElement->position;
}

auto GetMaximumVertexInDirection(std::span<const nc::Triangle> data, const nc::Vector3& direction) -> nc::Vector3
{
    NC_ASSERT(!data.empty(), "Vertex data is empty");
    auto maxDot = 0.0f;
    const nc::Vector3* maxElement = nullptr;

    for (const auto& triangle : data)
    {
        if (auto dot = nc::Dot(triangle.a, direction); dot > maxDot)
        {
            maxDot = dot;
            maxElement = &triangle.a;
        }
        if (auto dot = nc::Dot(triangle.b, direction); dot > maxDot)
        {
            maxDot = dot;
            maxElement = &triangle.b;
        }
        if (auto dot = nc::Dot(triangle.c, direction); dot > maxDot)
        {
            maxDot = dot;
            maxElement = &triangle.c;
        }
    }

    return maxElement ? *maxElement : nc::Vector3::Zero();
}

template<class T>
auto GetMeshVertexExtents(std::span<const T> data) -> nc::Vector3
{
    auto minX = GetMaximumVertexInDirection(data, nc::Vector3::Left());
    auto maxX = GetMaximumVertexInDirection(data, nc::Vector3::Right());
    auto minY = GetMaximumVertexInDirection(data, nc::Vector3::Down());
    auto maxY = GetMaximumVertexInDirection(data, nc::Vector3::Up());
    auto minZ = GetMaximumVertexInDirection(data, nc::Vector3::Back());
    auto maxZ = GetMaximumVertexInDirection(data, nc::Vector3::Front());

    if(minX.x > 0.0f || maxX.x < 0.0f || minY.y > 0.0f || maxY.y < 0.0f || minZ.z > 0.0f || maxZ.z < 0.0f)
    {
        LOG("Warning: Mesh does not contain the origin. The origin is assumed to be the center of mass.");
    }

    return nc::Vector3{ maxX.x - minX.x, maxY.y - minY.y, maxZ.z - minZ.z};
}
} // anonymous namespace

namespace nc::convert
{
auto FindFurthestDistanceFromOrigin(std::span<const Vector3> data) -> float
{
    NC_ASSERT(!data.empty(), "Vertex data is empty");
    auto maxElement = std::ranges::max_element(data, &CompareSquareMagnitudes);
    return Magnitude(*maxElement);
}

auto FindFurthestDistanceFromOrigin(std::span<const asset::MeshVertex> data) -> float
{
    NC_ASSERT(!data.empty(), "Vertex data is empty");
    auto maxElement = std::ranges::max_element(data, &CompareSquareMagnitudes, &PositionProjection);
    return Magnitude(maxElement->position);
}

auto FindFurthestDistanceFromOrigin(std::span<const Triangle> data) -> float
{
    NC_ASSERT(!data.empty(), "Triangle data is empty");
    auto maxDistance = 0.0f;
    for (const auto& tri : data)
    {
        if (auto dist = SquareMagnitude(tri.a); dist > maxDistance)
        {
            maxDistance = dist;
        }
        if (auto dist = SquareMagnitude(tri.b); dist > maxDistance)
        {
            maxDistance = dist;
        }
        if (auto dist = SquareMagnitude(tri.c); dist > maxDistance)
        {
            maxDistance = dist;
        }
    }

    return std::sqrt(maxDistance);
}

auto GetMeshVertexExtents(std::span<const Vector3> data) -> Vector3
{
    return ::GetMeshVertexExtents<Vector3>(data);
}

auto GetMeshVertexExtents(std::span<const asset::MeshVertex> data) -> Vector3
{
    return ::GetMeshVertexExtents<asset::MeshVertex>(data);
}

auto GetMeshVertexExtents(std::span<const Triangle> data) -> Vector3
{
    return ::GetMeshVertexExtents<Triangle>(data);
}
} // namespace nc::convert
