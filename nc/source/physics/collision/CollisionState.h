#pragma once

#include "narrow_phase/Polytope.h"
#include "directx/math/DirectXMath.h"

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
}