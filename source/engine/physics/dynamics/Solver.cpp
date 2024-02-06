#include "Solver.h"
#include "ecs/Registry.h"
#include "graphics/debug/DebugRenderer.h"
#include "physics/PhysicsConstants.h"

#include "optick.h"

#include <cassert>

using namespace DirectX;

namespace
{
using namespace nc;
using namespace nc::physics;

/** Default properties for entities without a PhysicsBody. */
const auto g_StaticPhysicsProperties = PhysicsProperties{.mass = 0.0f, .useGravity = false};
const auto g_StaticInverseInertia = XMMATRIX{g_XMZero, g_XMZero, g_XMZero, g_XMZero};

auto CreateContactConstraint(const Contact&, Transform*, Transform*, PhysicsBody*, PhysicsBody*) -> ContactConstraint;
void ResolveContactConstraint(ContactConstraint& constraint, float dt);
void ResolvePositionConstraint(PositionConstraint& constraint);
void ResolveJoint(Joint& joint);
auto ComputeLagrangeMultipliers(ContactConstraint& c, const ConstraintMatrix& v, float biasAlongNormal) -> Vector3;
auto ClampLambda(float newLambda, float* totalLambda) -> float;
auto ClampMu(float newMu, float extent, float* totalMu) -> float;

XMVECTOR ComputeLinearVelocity(FXMVECTOR jNorm, FXMVECTOR jTan, FXMVECTOR jBit, float invMass, float lambda, float muTangent, float muBitangent)
{
    return XMVectorScale(jNorm, invMass * lambda) +
           XMVectorScale(jTan, invMass * muTangent) +
           XMVectorScale(jBit, invMass * muBitangent);
}

XMVECTOR ComputeAngularVelocity(FXMVECTOR jNorm, FXMVECTOR jTan, FXMVECTOR jBit, FXMMATRIX invInertia, float lambda, float muTangent, float muBitangent)
{
    return XMVectorScale(XMVector3Transform(jNorm, invInertia), lambda) +
           XMVectorScale(XMVector3Transform(jTan, invInertia), muTangent) +
           XMVectorScale(XMVector3Transform(jBit, invInertia), muBitangent);
}

/** Compute largrange multipliers for contact constraint (1 for normal, 2 for friction).
 *  Result is returned as a vector: [lNorm, lTan, lBit] */
Vector3 ComputeLagrangeMultipliers(ContactConstraint& c, const ConstraintMatrix& v, float biasAlongNormal)
{
    OPTICK_CATEGORY("ComputeLagrangeMultipliers", Optick::Category::Physics);

    /** Compute JV in each direction. */
    XMVECTOR first = g_XMZero;
    if(c.physBodyA)
    {
        first = XMVector3Dot(c.jNormal.vA(), v.vA()) + XMVector3Dot(c.jNormal.wA(), v.wA());
        auto tan = XMVector3Dot(c.jTangent.vA(), v.vA()) + XMVector3Dot(c.jTangent.wA(), v.wA());
        auto bit = XMVector3Dot(c.jBitangent.vA(), v.vA()) + XMVector3Dot(c.jBitangent.wA(), v.wA());
        first = XMVectorMergeXY(first, tan);
        first = XMVectorPermute<XM_PERMUTE_0X, XM_PERMUTE_0Y, XM_PERMUTE_1X, XM_PERMUTE_1Y>(first, bit);
    }

    XMVECTOR second = g_XMZero;
    if(c.physBodyB)
    {
        second = XMVector3Dot(c.jNormal.vB(), v.vB()) + XMVector3Dot(c.jNormal.wB(), v.wB());
        auto tan = XMVector3Dot(c.jTangent.vB(), v.vB()) + XMVector3Dot(c.jTangent.wB(), v.wB());
        auto bit = XMVector3Dot(c.jBitangent.vB(), v.vB()) + XMVector3Dot(c.jBitangent.wB(), v.wB());
        second = XMVectorMergeXY(second, tan);
        second = XMVectorPermute<XM_PERMUTE_0X, XM_PERMUTE_0Y, XM_PERMUTE_1X, XM_PERMUTE_1Y>(second, bit);
    }

    /** lagrange = -(JV + b) / (J M^-1 J^t)^-1 = effMass [-(JV + b)] */
    auto bias = XMVectorSet(biasAlongNormal, 0.0f, 0.0f, 0.0f);
    auto lagrange_v = XMVectorNegate(first + second + bias) * c.effectiveMass;
    Vector3 lagrange;
    XMStoreVector3(&lagrange, lagrange_v);

    /** clamping = (Ln >= 0) && (-mu*Ln <= Lt <= mu*Ln) && (-mu*Ln <= Lb < = mu*Ln) */
    float maxFriction = c.friction * c.totalLambda;
    return Vector3
    {
        ClampLambda(lagrange.x, &c.totalLambda),
        ClampMu(lagrange.y, maxFriction, &c.totalMuTangent),
        ClampMu(lagrange.z, maxFriction, &c.totalMuBitangent)
    };
}

/** Clamping for impulse along normal. */
float ClampLambda(float newLambda, float* totalLambda)
{
    float temp = *totalLambda;
    *totalLambda = Max(0.0f, temp + newLambda);
    return *totalLambda - temp;
}

/** Clamping for impulse along tangents (friction). */
float ClampMu(float newMu, float extent, float* totalMu)
{
    float temp = *totalMu;
    *totalMu = Clamp(temp + newMu, -1.0f * extent, extent);
    return *totalMu - temp;
}

void Warmstart(const ContactConstraint& c, PhysicsBody* bodyA, PhysicsBody* bodyB)
{
    if (bodyA && !c.physBodyA->ParentEntity().IsStatic() && !c.physBodyA->IsKinematic())
    {
        bodyA->ApplyVelocities
        (
            ComputeLinearVelocity(c.jNormal.vA(), c.jTangent.vA(), c.jBitangent.vA(), c.invMassA, c.totalLambda, c.totalMuTangent, c.totalMuBitangent),
            ComputeAngularVelocity(c.jNormal.wA(), c.jTangent.wA(), c.jBitangent.wA(), c.invInertiaA, c.totalLambda, c.totalMuTangent, c.totalMuBitangent)
        );
    }

    if (bodyB && !c.physBodyB->ParentEntity().IsStatic() && !c.physBodyB->IsKinematic())
    {
        bodyB->ApplyVelocities
        (
            ComputeLinearVelocity(c.jNormal.vB(), c.jTangent.vB(), c.jBitangent.vB(), c.invMassB, c.totalLambda, c.totalMuTangent, c.totalMuBitangent),
            ComputeAngularVelocity(c.jNormal.wB(), c.jTangent.wB(), c.jBitangent.wB(), c.invInertiaB, c.totalLambda, c.totalMuTangent, c.totalMuBitangent)
        );
    }
}

ContactConstraint CreateContactConstraint(const Contact& contact,
                                          Transform* transformA,
                                          Transform* transformB,
                                          PhysicsBody* physBodyA,
                                          PhysicsBody* physBodyB)
{
    OPTICK_CATEGORY("CreateContactConstraint", Optick::Category::Physics);

    /** Compute vectors from object centers to contact points */
    auto rA = contact.worldPointA - transformA->Position();
    auto rB = contact.worldPointB - transformB->Position();
    auto rA_v = XMLoadVector3(&rA);
    auto rB_v = XMLoadVector3(&rB);

    /** Form orthonormal basis for friction vectors */
    auto normal_v = XMLoadVector3(&contact.normal);
    auto tangent_v = XMVector3Normalize(XMVector3Orthogonal(normal_v));
    auto bitangent_v = XMVector3Normalize(XMVector3Cross(normal_v, tangent_v));

    /** Compute Jacobians with linear and angular components for each direction
     *  J = [-d, -rA X d, d, rB X d] */
    auto jNormal = ConstraintMatrix::VelocityJacobian(rA_v, rB_v, normal_v);
    auto jTangent = ConstraintMatrix::VelocityJacobian(rA_v, rB_v, tangent_v);
    auto jBitangent = ConstraintMatrix::VelocityJacobian(rA_v, rB_v, bitangent_v);

    /** Get instance properties or static properties for both entities */
    auto GetPhysicsProperties = [](const PhysicsBody* body) -> std::tuple<FXMMATRIX, float, float, float>
    {
        if(body)
            return {body->GetInverseInertia(), body->GetInverseMass(), body->GetRestitution(), body->GetFriction()};

        return {g_StaticInverseInertia, g_StaticPhysicsProperties.mass, g_StaticPhysicsProperties.restitution, g_StaticPhysicsProperties.friction};
    };

    const auto [invInertiaA, invMassA, restitutionA, frictionA] = GetPhysicsProperties(physBodyA);
    const auto [invInertiaB, invMassB, restitutionB, frictionB] = GetPhysicsProperties(physBodyB);

    /** Compute effective mass for each impulse type */
    auto jInertiaProductA = XMVector3Transform(jNormal.wA(), invInertiaA);
    auto jInertiaProductB = XMVector3Transform(jNormal.wB(), invInertiaB);
    auto dotNormal = XMVector3Dot(jNormal.wA(), jInertiaProductA) + XMVector3Dot(jNormal.wB(), jInertiaProductB);

    jInertiaProductA = XMVector3Transform(jTangent.wA(), invInertiaA);
    jInertiaProductB = XMVector3Transform(jTangent.wB(), invInertiaB);
    auto dotTangent = XMVector3Dot(jTangent.wA(), jInertiaProductA) + XMVector3Dot(jTangent.wB(), jInertiaProductB);

    jInertiaProductA = XMVector3Transform(jBitangent.wA(), invInertiaA);
    jInertiaProductB = XMVector3Transform(jBitangent.wB(), invInertiaB);
    auto dotBitangent = XMVector3Dot(jBitangent.wA(), jInertiaProductA) + XMVector3Dot(jBitangent.wB(), jInertiaProductB);

    auto effectiveMass = XMVectorMergeXY(dotNormal, dotTangent);
    effectiveMass = XMVectorPermute<XM_PERMUTE_0X, XM_PERMUTE_0Y, XM_PERMUTE_1X, XM_PERMUTE_1Y>(effectiveMass, dotBitangent);
    effectiveMass = effectiveMass + XMVectorReplicate(invMassA + invMassB);
    effectiveMass = XMVectorReciprocal(effectiveMass);

    /** Combined restitution and friction */
    float restitution = restitutionA * restitutionB;
    float friction = frictionA * frictionB;

    auto constraint = ContactConstraint
    {
        physBodyA, physBodyB,
        jNormal, jTangent, jBitangent,
        invInertiaA, invInertiaB,
        normal_v, rA_v, rB_v, effectiveMass,
        contact.depth,
        invMassA, invMassB,
        restitution, friction,
        0.0f, 0.0f, 0.0f
    };

    /** Warmstart accumulated impulses */
    if constexpr(EnableContactWarmstarting)
    {
        constraint.totalLambda = contact.lambda * WarmstartFactor;
        constraint.totalMuTangent = contact.muTangent * WarmstartFactor;
        constraint.totalMuBitangent = contact.muBitangent * WarmstartFactor;
        Warmstart(constraint, physBodyA, physBodyB);
    }

    return constraint;
}

/** Resolve contacts through sequential impulse. */
void ResolveContactConstraint(ContactConstraint& constraint, float dt)
{
    OPTICK_CATEGORY("ResolveContactConstraint", Optick::Category::Physics);

    /** V = [Va, Wa, Vb, Wb] */
    ConstraintMatrix v
    {
        constraint.physBodyA ? constraint.physBodyA->GetVelocity() : g_XMZero,
        constraint.physBodyA ? constraint.physBodyA->GetAngularVelocity() : g_XMZero,
        constraint.physBodyB ? constraint.physBodyB->GetVelocity() : g_XMZero,
        constraint.physBodyB ? constraint.physBodyB->GetAngularVelocity() : g_XMZero
    };

    /** Baumgarte Stabilization / Restitution
     *  bias = b / h (Pa-Pb) * n + Cr(Vb + Wb X Rb - Va - Wa X Ra) * n */
    auto relativeVelocity_v = v.vB() + XMVector3Cross(v.wB(), constraint.rB) - v.vA() - XMVector3Cross(v.wA(), constraint.rA);
    relativeVelocity_v = XMVector3Dot(relativeVelocity_v, constraint.normal);
    float restitution = XMVectorGetX(relativeVelocity_v);

    if constexpr(EnableRestitutionSlop)
    {
        restitution = Max(restitution - RestitutionSlop, 0.0f);
    }

    auto bias = restitution * constraint.restitution;

    if constexpr(EnableBaumgarteStabilization)
    {
        bias += Max(constraint.penetrationDepth - PenetrationSlop, 0.0f) * -1.0f * BaumgarteFactor / dt;
    }

    /** Compute impulse factors */
    auto [lagrangeNormal, lagrangeTangent, lagrangeBitangent] = ComputeLagrangeMultipliers(constraint, v, bias);

    if(constraint.physBodyA && !constraint.physBodyA->ParentEntity().IsStatic() && !constraint.physBodyA->IsKinematic())
    {
        auto linear = ComputeLinearVelocity(constraint.jNormal.vA(), constraint.jTangent.vA(), constraint.jBitangent.vA(), constraint.invMassA, lagrangeNormal, lagrangeTangent, lagrangeBitangent);
        auto angular = ComputeAngularVelocity(constraint.jNormal.wA(), constraint.jTangent.wA(), constraint.jBitangent.wA(), constraint.invInertiaA, lagrangeNormal, lagrangeTangent, lagrangeBitangent);
        constraint.physBodyA->ApplyVelocities(linear, angular);
    }

    if(constraint.physBodyB && !constraint.physBodyB->ParentEntity().IsStatic() && !constraint.physBodyB->IsKinematic())
    {
        auto linear = ComputeLinearVelocity(constraint.jNormal.vB(), constraint.jTangent.vB(), constraint.jBitangent.vB(), constraint.invMassB, lagrangeNormal, lagrangeTangent, lagrangeBitangent);
        auto angular = ComputeAngularVelocity(constraint.jNormal.wB(), constraint.jTangent.wB(), constraint.jBitangent.wB(), constraint.invInertiaB, lagrangeNormal, lagrangeTangent, lagrangeBitangent);
        constraint.physBodyB->ApplyVelocities(linear, angular);
    }
}

void ResolvePositionConstraint(PositionConstraint& constraint)
{
    OPTICK_CATEGORY("ResolvePositionConstraint", Optick::Category::Physics);

    if(constraint.depth < PenetrationSlop)
        return;

    const auto mtv = constraint.normal * (constraint.depth * PositionCorrectionFactor);

    /** If only one body can move, the mtv is doubled so the total translation is the same
     *  as the two body case. */
    if(constraint.eventType == CollisionEventType::FirstBodyPhysics)
    {
        constraint.transformA->Translate(mtv * -2.0f);
        return;
    }

    if(constraint.eventType == CollisionEventType::SecondBodyPhysics)
    {
        constraint.transformB->Translate(mtv * 2.0f);
        return;
    }

    constraint.transformA->Translate(-mtv);
    constraint.transformB->Translate(mtv);
}

void ResolveJoint(Joint& joint)
{
    OPTICK_CATEGORY("ResolveJoint", Optick::Category::Physics);

    const auto& vA = joint.bodyA->GetVelocity();
    const auto& wA = joint.bodyA->GetAngularVelocity();
    const auto& vB = joint.bodyB->GetVelocity();
    const auto& wB = joint.bodyB->GetAngularVelocity();

    const auto relativeVelocity = vB + XMVector3Cross(wB, joint.rB) - vA - XMVector3Cross(wA, joint.rA);
    const auto softness = XMVectorScale(joint.p, joint.softness);
    const auto impulse = XMVector3Transform(joint.bias - relativeVelocity - softness, joint.m);
    joint.p += impulse;

    /** dV = +/- (impulse / mass)
     *  dW = +/- (r X impulse) * I^-1 */
    joint.bodyA->ApplyVelocities
    (
        XMVectorNegate(XMVectorScale(impulse, joint.bodyA->GetInverseMass())),
        XMVectorNegate(XMVector3Transform(XMVector3Cross(joint.rA, impulse), joint.bodyA->GetInverseInertia()))
    );

    joint.bodyB->ApplyVelocities
    (
        XMVectorScale(impulse, joint.bodyB->GetInverseMass()),
        XMVector3Transform(XMVector3Cross(joint.rB, impulse), joint.bodyB->GetInverseInertia())
    );
}
} // anonymous namespace

namespace nc::physics
{
Solver::Solver(Registry* registry)
    : m_registry{registry},
        m_contactConstraints{},
        m_positionConstraints{}
{
}

void Solver::GenerateConstraints(std::span<const Manifold> manifolds)
{
    OPTICK_CATEGORY("GenerateConstraints", Optick::Category::Physics);

#if NC_PHYSICS_DRAW_CONTACT_POINTS
    graphics::DebugRenderer::ClearPoints();
#endif

    const auto manifoldCount = manifolds.size();
    m_contactConstraints.clear();
    m_contactConstraints.reserve(manifoldCount * 4u);

    if constexpr(EnableDirectPositionCorrection)
    {
        m_positionConstraints.clear();
        m_positionConstraints.reserve(manifoldCount);
    }

    for(const auto& manifold : manifolds)
    {
        const Entity entityA = manifold.Event().first;
        const Entity entityB = manifold.Event().second;
        auto* transformA = m_registry->Get<Transform>(entityA);
        auto* transformB = m_registry->Get<Transform>(entityB);
        auto* physBodyA = m_registry->Get<PhysicsBody>(entityA);
        auto* physBodyB = m_registry->Get<PhysicsBody>(entityB);

        if constexpr(EnableDirectPositionCorrection)
        {
            const auto& deepestContact = manifold.DeepestContact();
            m_positionConstraints.emplace_back(transformA, transformB, deepestContact.normal, deepestContact.depth, manifold.Event().eventType);
        }

        for(const auto& contact : manifold.Contacts())
        {
            #if NC_PHYSICS_DRAW_CONTACT_POINTS
            graphics::DebugRenderer::AddPoint(contact.worldPointA);
            graphics::DebugRenderer::AddPoint(contact.worldPointB);
            #endif

            m_contactConstraints.push_back(CreateContactConstraint(contact, transformA, transformB, physBodyA, physBodyB));
        }
    }
}

void Solver::ResolveConstraints(std::span<Joint> joints, float dt)
{
    OPTICK_CATEGORY("ResolveConstraints", Optick::Category::Physics);

    for(size_t i = 0u; i < SolverIterations; ++i)
    {
        for(auto& constraint : m_contactConstraints)
        {
            ResolveContactConstraint(constraint, dt);
        }

        for(auto& joint : joints)
        {
            ResolveJoint(joint);
        }
    }

    if constexpr(EnableDirectPositionCorrection)
    {
        for(auto& constraint : m_positionConstraints)
        {
            ResolvePositionConstraint(constraint);
        }
    }
}
} // namespace nc::physics
