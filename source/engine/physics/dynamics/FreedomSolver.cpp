#include "FreedomSolver.h"

#include <ranges>

namespace
{
struct SoftConstraintTerms
{
    float beta;
    float gamma;
};

auto ComputeSoftConstraintTerms(float dampingRatio, float dampingFrequency, float effectiveMass, float dt) -> SoftConstraintTerms
{
    // beta = hk / (c + hk)  (position bias)
    // gamma = 1 / (c + hk)  (softness)
    // k = (w^2)m            (spring stiffness)
    // c = 2zwm              (damping factor)
    const auto zeta = dampingRatio;
    const auto omega = 2.0f * 3.14f * dampingFrequency;
    const auto k = effectiveMass * omega * omega;
    const auto c = 2.0f * effectiveMass * zeta * omega;
    return SoftConstraintTerms{
        .beta = (k * dt) / (c + k * dt),
        .gamma = 1.0f / (c + k * dt)
    };
}
} // anonymous namespace

namespace nc::physics
{
auto GeneratePositionClampConstraints(ecs::ExplicitEcs<PositionClamp, PhysicsBody, Transform> gameState, float dt) -> std::vector<PositionClampConstraint>
{
    using namespace DirectX;
    auto& pool = gameState.GetPool<PositionClamp>();
    auto out = std::vector<PositionClampConstraint>{};
    out.reserve(pool.size());
    for (auto [entity, clamp] : std::views::zip(pool.GetEntityPool(), pool.GetComponents()))
    {
        if (!gameState.Contains<PhysicsBody>(entity))
            continue;

        auto& body = gameState.Get<PhysicsBody>(entity);
        const auto invMass = body.GetInverseMass();
        const auto& position = gameState.Get<Transform>(entity).PositionXM();

        // Soft constraints model a spring. We can precompute pieces of bias + softness terms.
        // 0 = JV + bias + softness = JV + (beta/h) posError + gamma * totalLambda
        const auto effMass = invMass == 0.0f ? FLT_MAX : 1.0f / invMass;
        const auto [beta, gamma] = ComputeSoftConstraintTerms(clamp.dampingRatio, clamp.dampingFrequency, effMass, dt);

        out.emplace_back(
            position,
            ToXMVector(clamp.targetPosition),
            &body,
            beta,
            gamma / dt,
            0.0f
        );
    }

    return out;
}

auto GenerateVelocityRestrictionConstraints(ecs::ExplicitEcs<VelocityRestriction, PhysicsBody, Transform> gameState) -> std::vector<VelocityRestrictionConstraint>
{
    using namespace DirectX;
    auto& pool = gameState.GetPool<VelocityRestriction>();
    auto out = std::vector<VelocityRestrictionConstraint>{};
    out.reserve(pool.size());
    for (auto [entity, restriction] : std::views::zip(pool.GetEntityPool(), pool.GetComponents()))
    {
        if (!gameState.Contains<PhysicsBody>(entity))
            continue;

        auto& body = gameState.Get<PhysicsBody>(entity);
        const auto& rotation = restriction.worldSpace
            ? g_XMZero
            : gameState.Get<Transform>(entity).RotationXM();

        out.emplace_back(
            XMLoadVector3(&restriction.linearFreedom),
            XMLoadVector3(&restriction.angularFreedom),
            rotation,
            &body,
            restriction.worldSpace
        );
    }

    return out;
}

void Solve(std::vector<PositionClampConstraint>& constraints, float dt)
{
    // With softness term, constraint looks like this (note that *total* lambda feeds back in):
    //     JV + (beta/dt) C(x) + gamma * totalLambda = 0

    using namespace DirectX;
    for (auto& constraint : constraints)
    {
        const auto& velocity = constraint.body->GetVelocity();
        const auto invMass = constraint.body->GetInverseMass();

        // Compute new bias/normal each solver iteration by integrating with current velocity
        const auto tentativePosition = XMVectorAdd(constraint.position, XMVectorScale(velocity, dt));
        const auto offset = XMVectorSubtract(tentativePosition, constraint.targetPosition);
        const auto normal = XMVector3Normalize(offset);
        const auto positionError = XMVectorGetX(XMVector3Length(offset));
        const auto bias = constraint.beta / dt * positionError;

        // Solve for lambda in the usual way, but with gamma added to effective mass
        //     lambda = (effMass + gamma) * -(JV + B)
        const auto jv = XMVectorGetX(XMVector3Dot(velocity, normal));
        const auto lambda = -(jv + bias + constraint.softness * constraint.totalLambda) / (invMass + constraint.softness);
        constraint.totalLambda += lambda;
        constraint.body->ApplyVelocity(XMVectorScale(normal, lambda * invMass));
    }
}

void Solve(std::vector<VelocityRestrictionConstraint>& constraints)
{
    using namespace DirectX;
    for (auto& constraint : constraints)
    {
        const auto& linearVelocity = constraint.body->GetVelocity();
        const auto& angularVelocity = constraint.body->GetAngularVelocity();
        if (constraint.worldSpace)
        {
            constraint.body->SetVelocities(
                XMVectorMultiply(linearVelocity, constraint.linearFreedom),
                XMVectorMultiply(angularVelocity, constraint.angularFreedom)
            );
            continue;
        }

        // Rotate velocities to local space before restricting. Note to self: The freedom values are
        // damping coefficients, not coordinate vectors. Don't try and optimize this by rotating the
        // freedom vectors instead (I know you will!).
        auto localLinear = XMVector3InverseRotate(linearVelocity, constraint.rotation);
        localLinear = XMVectorMultiply(localLinear, constraint.linearFreedom);
        auto localAngular = XMVector3InverseRotate(angularVelocity, constraint.rotation);
        localAngular = XMVectorMultiply(localAngular, constraint.angularFreedom);
        constraint.body->SetVelocities(
            XMVector3Rotate(localLinear, constraint.rotation),
            XMVector3Rotate(localAngular, constraint.rotation)
        );
    }
}
} // namespace nc::physics
