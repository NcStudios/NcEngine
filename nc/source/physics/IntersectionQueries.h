#pragma once

#include "CollisionVolumes.h"

namespace nc::physics
{
    /* Test intersection between two transformed spheres. */
    bool Intersect(const SphereCollider& a, const SphereCollider& b);

    /* Test intersection between transformed sphere and axis-aligned bounding box. */
    bool Intersect(const SphereCollider& sphere, const BoxCollider& aabb);

    /* Intersection query between any two untransformed volumes. */
    bool Gjk(const BoundingVolume& a, const BoundingVolume& b, DirectX::FXMMATRIX aMatrix, DirectX::FXMMATRIX bMatrix);
            
    /* Intersection query between any two transformed volumes. */
    bool Gjk(const BoundingVolume& a, const BoundingVolume& b);

    /* Get square minimum translation distance from a point to an axis-aligned bounding box. */
    float SquareMtdToAABB(const Vector3& point, const BoxCollider& aabb);
} // namespace nc::physics