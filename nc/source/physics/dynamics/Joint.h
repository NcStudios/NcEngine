#pragma once

#include "ecs/Registry.h"

namespace nc::physics
{
    struct Joint
    {
        Entity entityA;
        Entity entityB;
        PhysicsBody* bodyA;
        PhysicsBody* bodyB;
        DirectX::XMVECTOR anchorA;
        DirectX::XMVECTOR anchorB;
        DirectX::XMMATRIX m;
        DirectX::XMVECTOR rA, rB, bias, p;
        float biasFactor, softness;
    };
}