#pragma once

#include "CollisionState.h"
#include "physics/Collider.h"

namespace nc::physics
{
enum class HalfspaceContainment
{
    Intersecting,
    Positive,
    Negative
};

/** Test intersection between untransformed volumes. */
bool Intersect(const BoundingVolume& a, const BoundingVolume& b, DirectX::FXMMATRIX aMatrix, DirectX::FXMMATRIX bMatrix);

/** Test intersection between transformed volumes. */
bool Intersect(const BoundingVolume& a, const BoundingVolume& b);
bool Intersect(const Sphere& a, const Sphere& b);
bool Intersect(const Frustum& a, const Sphere& b);

/** Test intersection and generate contact points between untransformed volumes. */
bool Collide(const BoundingVolume& a, const BoundingVolume& b, DirectX::FXMMATRIX aMatrix, DirectX::FXMMATRIX bMatrix, CollisionState* state);

/** Test intersection and generate contact points between an untransformed volume and transformed triangle. */
bool Collide(const BoundingVolume& a, const Triangle& b, DirectX::FXMMATRIX aMatrix, CollisionState* stateOut);

/** Test a sphere's location relative to a plane. */
auto TestHalfspace(const Plane& plane, const Sphere& sphere) -> HalfspaceContainment;

inline bool Intersect(const Sphere& a, const Sphere& b)
{
    auto radii = a.radius + b.radius;
    return SquareMagnitude(a.center - b.center) < radii * radii;
}
} // namespace nc::physics
