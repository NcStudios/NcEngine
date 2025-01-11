#pragma once

#include "ncjolt/JoltApi.h"
#include "ncjolt/ShapeUtility.h"
#include "ncmath/Geometry.h"

#include "Jolt/Jolt.h"
#include "Jolt/Physics/Collision/Shape/ConvexHullShape.h"

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

inline auto UpcastToConvexHull(JPH::Shape* shape) -> JPH::ConvexHullShape*
{
    if (shape->GetSubType() != JPH::EShapeSubType::ConvexHull)
    {
        ADD_FAILURE() << "Shape not a ConvexHull";
        return nullptr;
    }

    return static_cast<JPH::ConvexHullShape*>(shape);
}
