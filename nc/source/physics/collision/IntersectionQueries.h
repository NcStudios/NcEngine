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

    /** Test intersection between untransformed volumes. */
    bool Intersect(const BoundingVolume& a, const BoundingVolume& b, DirectX::FXMMATRIX aMatrix, DirectX::FXMMATRIX bMatrix);

    /** Test intersection between transformed volumes. */
    bool Intersect(const BoundingVolume& a, const BoundingVolume& b);
    bool Intersect(const Sphere& a, const Sphere& b);

    /** Test intersection and generate contact points between untransformed volumes. */
    bool Collide(const BoundingVolume& a, const BoundingVolume& b, DirectX::FXMMATRIX aMatrix, DirectX::FXMMATRIX bMatrix, CollisionState* state);
    
    /** Test intersection and generate contact points between an untransformed volume and transformed triangle. */
    bool Collide(const BoundingVolume& a, const Triangle& b, DirectX::FXMMATRIX aMatrix, CollisionState* stateOut);
} // namespace nc::physics