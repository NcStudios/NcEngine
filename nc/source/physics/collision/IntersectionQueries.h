#pragma once

#include "component/Collider.h"
#include "Simplex.h"
#include "Polytope.h"

namespace nc::physics
{
    /* Collision data produced by Gjk and consumed by Epa. */
    struct CollisionState
    {
        Simplex simplex;
        Polytope polytope;
        Contact contact;
        DirectX::XMVECTOR rotationA;
        DirectX::XMVECTOR rotationB;
    };

    /* Intersection query between any two untransformed volumes. */
    bool Gjk(const BoundingVolume& a, const BoundingVolume& b, DirectX::FXMMATRIX aMatrix, DirectX::FXMMATRIX bMatrix, CollisionState* stateOut);

    /* Generate contact data from output of Gjk. */
    bool Epa(const BoundingVolume& a, const BoundingVolume& b, DirectX::FXMMATRIX aMatrix, DirectX::FXMMATRIX bMatrix, CollisionState* state);

    /* Collision/contact generation for volumes against static mesh triangles. */
    bool Gjk(const BoundingVolume& a, const Triangle& b, DirectX::FXMMATRIX aMatrix, CollisionState* stateOut);
    bool Epa(const BoundingVolume& a, const Triangle& b, DirectX::FXMMATRIX aMatrix, CollisionState* stateOut);

    /* Intersection queries between transformed volumes. */
    bool Intersect(const BoundingVolume& a, const BoundingVolume& b);
    bool Intersect(const Sphere& a, const Sphere& b);
    bool Intersect(const Sphere& sphere, const Box& aabb);

    /* Get square minimum translation distance from a point to an axis-aligned bounding box. */
    float SquareMtdToAABB(const Vector3& point, const Box& aabb);
} // namespace nc::physics