#pragma once

#include "ncengine/ecs/Ecs.h"
#include "ncengine/physics/FreedomConstraint.h"
#include "ncengine/physics/PhysicsBody.h"

namespace nc::physics
{
struct VelocityRestrictionConstraint
{
    DirectX::XMVECTOR linearFreedom;
    DirectX::XMVECTOR angularFreedom;
    DirectX::XMVECTOR rotation;
    PhysicsBody* body;
    bool worldSpace;
};

struct PositionClampConstraint
{
    DirectX::XMVECTOR position;
    DirectX::XMVECTOR targetPosition;
    PhysicsBody* body;
    float beta;
    float softness;
    float totalLambda;
};

auto GeneratePositionClampConstraints(ecs::ExplicitEcs<PositionClamp, PhysicsBody, Transform> gameState, float dt) -> std::vector<PositionClampConstraint>;
auto GenerateVelocityRestrictionConstraints(ecs::ExplicitEcs<VelocityRestriction, PhysicsBody, Transform> gameState) -> std::vector<VelocityRestrictionConstraint>;

void Solve(std::vector<PositionClampConstraint>& constraints, float dt);
void Solve(std::vector<VelocityRestrictionConstraint>& constraints);
} // namespace nc::physics
