#pragma once

#include "ncengine/ecs/Ecs.h"
#include "ncengine/physics/FreedomConstraint.h"
#include "ncengine/physics/PhysicsBody.h"

#include <ranges>

namespace nc::physics
{
void SolveFreedomConstraints(ecs::ExplicitEcs<FreedomConstraint, PhysicsBody, Transform> gameState)
{
    const auto& constraintPool = gameState.GetPool<FreedomConstraint>();
    for (const auto [entity, constraint] : std::views::zip(constraintPool.GetEntityPool(), constraintPool.GetComponents()))
    {
        if (!gameState.Contains<PhysicsBody>(entity))
            continue;

        auto& body = gameState.Get<PhysicsBody>(entity);
        if (constraint.worldSpace)
        {
            body.SetVelocities
            (
                DirectX::XMVectorMultiply(body.GetVelocity(), constraint.linearFreedom),
                DirectX::XMVectorMultiply(body.GetAngularVelocity(), constraint.angularFreedom)
            );
            continue;
        }

        const auto& rotation = gameState.Get<Transform>(entity).RotationXM();
        auto localLinearVelocity = DirectX::XMVector3InverseRotate(body.GetVelocity(), rotation);
        auto localAngularVelocity = DirectX::XMVector3InverseRotate(body.GetAngularVelocity(), rotation);
        localLinearVelocity = DirectX::XMVectorMultiply(localLinearVelocity, constraint.linearFreedom);
        localAngularVelocity = DirectX::XMVectorMultiply(localAngularVelocity, constraint.angularFreedom);
        body.SetVelocities
        (
            DirectX::XMVector3Rotate(localLinearVelocity, rotation),
            DirectX::XMVector3Rotate(localAngularVelocity, rotation)
        );
    }
}
} // namespace nc::physics
