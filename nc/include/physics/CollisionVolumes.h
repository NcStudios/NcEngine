#pragma once

#include "math/Vector.h"

#include <span>
#include <variant>

namespace nc
{
    struct SphereCollider
    {
        Vector3 center;
        float radius;
    };

    struct BoxCollider
    {
        Vector3 center;
        Vector3 extents;
        float maxExtent;
    };

    struct CapsuleCollider
    {
        Vector3 pointA, pointB;
        float radius;
        float maxExtent;
    };

    struct HullCollider
    {
        std::span<const Vector3> vertices;
        Vector3 extents;
        float maxExtent;
    };

    struct MeshCollider
    {
        struct Triangle
        {
            Vector3 a, b, c, normal;
        };

        std::span<const Triangle> triangleList;
    };

    using BoundingVolume = std::variant<BoxCollider, SphereCollider, CapsuleCollider, HullCollider, MeshCollider>;
}