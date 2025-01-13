#pragma once

#include "ncjolt/JoltApi.h"
#include "ncjolt/ShapeUtility.h"
#include "ncmath/Geometry.h"

#include "Jolt/Jolt.h"
#include "Jolt/Physics/Collision/Shape/ConvexHullShape.h"
#include "Jolt/Physics/Collision/Shape/MeshShape.h"

#include <ranges>

namespace nc
{
inline auto operator==(const Triangle& lhs, const Triangle& rhs) -> bool
{
    return lhs.a == rhs.a && lhs.b == rhs.b && lhs.c == rhs.c;
}
} // namespace nc

inline auto ToVector3(const JPH::Vec3& in) -> nc::Vector3
{
    return nc::Vector3{in.GetX(), in.GetY(), in.GetZ()};
}

inline auto GetTriangles(const JPH::MeshShape& shape, size_t maxTriangles) -> std::vector<nc::Triangle>
{
    // There doesn't seem to be an easy way to get triangles out of a MeshShape. Resort to collision, for tests.
    constexpr auto querySize = JPH::Shape::cGetTrianglesMinTrianglesRequested;
    const auto queryWithin = shape.GetLocalBounds();
    auto ctx = JPH::MeshShape::GetTrianglesContext{};
    auto vertices = std::vector<JPH::Float3>(maxTriangles * 3);
    auto triangleCount = 0;
    shape.GetTrianglesStart(ctx, queryWithin, JPH::Vec3::sZero(), JPH::Quat::sIdentity(), JPH::Vec3::sReplicate(1.0f));
    while (true)
    {
        auto out = vertices.data() + triangleCount * 3;
        const auto numFound = shape.GetTrianglesNext(ctx, querySize, out, nullptr);
        if (numFound == 0)
        {
            break;
        }

        triangleCount += numFound;
    }

    auto toTriangle = [](const auto& rng){
        return nc::Triangle{
            reinterpret_cast<const nc::Vector3&>(rng[0]),
            reinterpret_cast<const nc::Vector3&>(rng[1]),
            reinterpret_cast<const nc::Vector3&>(rng[2])
        };
    };

    return std::views::chunk(vertices, 3)
         | std::views::transform(toTriangle)
         | std::ranges::to<std::vector<nc::Triangle>>();
}

inline auto UpcastToConvexHull(JPH::Shape* shape) -> JPH::ConvexHullShape*
{
    if (shape->GetSubType() != JPH::EShapeSubType::ConvexHull)
    {
        ADD_FAILURE() << "Shape not a ConvexHull";
        return nullptr;
    }

    return static_cast<JPH::ConvexHullShape*>(shape);
}

inline auto UpcastToMeshShape(JPH::Shape* shape) -> JPH::MeshShape*
{
    if (shape->GetSubType() != JPH::EShapeSubType::Mesh)
    {
        ADD_FAILURE() << "Shape not a MeshShape";
        return nullptr;
    }

    return static_cast<JPH::MeshShape*>(shape);
}
