#pragma once

#include "narrow_phase/Polytope.h"
#include "DirectXMath.h"

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

} // namespace nc::physics