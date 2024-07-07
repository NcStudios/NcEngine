#pragma once

#include "ncengine/ecs/EcsFwd.h"

#include "DirectXMath.h"

#include <vector>

namespace nc
{
class Transform;

namespace physics
{
class PhysicsBody;
struct OrientationClamp;
struct PositionClamp;
struct VelocityRestriction;

struct VelocityRestrictionConstraint
{
    DirectX::XMVECTOR linearFreedom;
    DirectX::XMVECTOR angularFreedom;
    DirectX::XMVECTOR rotation;
    PhysicsBody* body;
    bool worldSpace;
};

struct OrientationClampConstraint
{
    DirectX::XMVECTOR currentUp;
    DirectX::XMVECTOR targetUp;
    DirectX::XMVECTOR axisOfRotation;
    PhysicsBody* body;
    float effectiveMass;
    float beta;
    float softness;
    float totalLambda;
};

struct PositionClampConstraint
{
    DirectX::XMVECTOR position;
    DirectX::XMVECTOR targetPosition;
    PhysicsBody* body;
    float beta;
    float softness;
    float slackDistance;
    float slackDamping;
    float totalLambda;
};

auto GenerateOrientationClampConstraints(ecs::ExplicitEcs<OrientationClamp, PhysicsBody, Transform> gameState, float dt) -> std::vector<OrientationClampConstraint>;
auto GeneratePositionClampConstraints(ecs::ExplicitEcs<PositionClamp, PhysicsBody, Transform> gameState, float dt) -> std::vector<PositionClampConstraint>;
auto GenerateVelocityRestrictionConstraints(ecs::ExplicitEcs<VelocityRestriction, PhysicsBody, Transform> gameState) -> std::vector<VelocityRestrictionConstraint>;

void Solve(std::vector<OrientationClampConstraint>& constraints, float dt);
void Solve(std::vector<PositionClampConstraint>& constraints, float dt);
void Solve(std::vector<VelocityRestrictionConstraint>& constraints);
} // namespace physics
} // namespace nc
