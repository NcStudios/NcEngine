#pragma once

#include "CollisionVolumes.h"
#include "Simplex.h"
#include "Polytope.h"

namespace nc::physics
{
    /* Collision data produced by Gjk and consumed by Epa. */
    struct CollisionState
    {
        Simplex simplex;
        Polytope polytope;
        DirectX::XMVECTOR rotationA;
        DirectX::XMVECTOR rotationB;
    };

    /* Intersection query between any two untransformed volumes. */
    bool Gjk(const BoundingVolume& a, const BoundingVolume& b, DirectX::FXMMATRIX aMatrix, DirectX::FXMMATRIX bMatrix, CollisionState* stateOut);

    /* Generate contact data from output of Gjk. */
    bool Epa(const BoundingVolume& a, const BoundingVolume& b, DirectX::FXMMATRIX aMatrix, DirectX::FXMMATRIX bMatrix, CollisionState* state, Contact* contact);

    /* Intersection queries between transformed volumes. */
    bool Intersect(const BoundingVolume& a, const BoundingVolume& b);
    bool Intersect(const SphereCollider& a, const SphereCollider& b);
    bool Intersect(const SphereCollider& sphere, const BoxCollider& aabb);

    /* Get square minimum translation distance from a point to an axis-aligned bounding box. */
    float SquareMtdToAABB(const Vector3& point, const BoxCollider& aabb);
} // namespace nc::physics