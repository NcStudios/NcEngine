#include "Solver.h"
#include "ecs/Registry.h"
#include "physics/PhysicsConstants.h"
#include "graphics/DebugRenderer.h"
#include "debug/Profiler.h"
//#define USE_DEBUG_RENDERING
#undef USE_DEBUG_RENDERING

#include <cassert>

using namespace DirectX;


#include "time/NcTime.h"
#include <iostream>

namespace
{
    using namespace nc;
    using namespace nc::physics;

    /** Default properties for entities without a PhysicsBody. */
    const auto g_StaticPhysicsProperties = PhysicsProperties{.mass = 0.0f, .useGravity = false};
    const auto g_StaticInverseInertia = XMMATRIX{g_XMZero, g_XMZero, g_XMZero, g_XMZero};

    auto CreateContactConstraint(const Contact&, Entity, Entity, Transform*, Transform*, PhysicsBody*, PhysicsBody*) -> ContactConstraint;
    void ResolveContactConstraint(ContactConstraint& constraint, float dt);
    void ResolvePositionConstraint(PositionConstraint& constraint);
    void ResolveJoint(Joint& joint);
    auto MultiplyJVContact(const ConstraintMatrix& v, const ConstraintMatrix& jNormal, const ConstraintMatrix& jTangent, const ConstraintMatrix& jBitangent) -> XMVECTOR;
    auto ComputeDeltas(const ContactConstraint& constraint, float lNormal, float lTangent, float lBitangent) -> ConstraintMatrix;
    auto ClampLambda(float newLambda, float* totalLambda) -> float;
    auto ClampMu(float newMu, float extent, float* totalMu) -> float;

    ContactConstraint CreateContactConstraint(const Contact& contact,
                                              Entity entityA,
                                              Entity entityB,
                                              Transform* transformA,
                                              Transform* transformB,
                                              PhysicsBody* physBodyA,
                                              PhysicsBody* physBodyB)
    {
        /** Compute vectors from object centers to contact points */
        auto rA = contact.worldPointA - transformA->GetPosition();
        auto rB = contact.worldPointB - transformB->GetPosition();
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

        /** Warmstart accumulated impulses */
        float lambda, muTangent, muBitangent;
        if constexpr(EnableContactWarmstarting)
        {
            lambda = contact.lambda * WarmstartFactor;
            muTangent = contact.muTangent * WarmstartFactor;
            muBitangent = contact.muBitangent * WarmstartFactor;
        }
        else
        {
            lambda = 0.0f;
            muTangent = 0.0f;
            muBitangent = 0.0f;
        }

        return ContactConstraint
        {
            entityA, entityB,
            physBodyA, physBodyB,
            jNormal, jTangent, jBitangent,
            invInertiaA, invInertiaB,
            normal_v, rA_v, rB_v, effectiveMass,
            contact.depth,
            invMassA, invMassB,
            restitution, friction,
            lambda, muTangent, muBitangent
        };
    }

    /** Resolve contacts through sequential impulse. */
    void ResolveContactConstraint(ContactConstraint& constraint, float dt)
    {
        /** V = [Va, Wa, Vb, Wb] */
        ConstraintMatrix v
        {
            constraint.physBodyA ? constraint.physBodyA->GetVelocity() : g_XMZero,
            constraint.physBodyA ? constraint.physBodyA->GetAngularVelocity() : g_XMZero,
            constraint.physBodyB ? constraint.physBodyB->GetVelocity() : g_XMZero,
            constraint.physBodyB ? constraint.physBodyB->GetAngularVelocity() : g_XMZero
        };

        /** Baumgarte Stabilization / Restitution
         *  bias = b / h (Pa-Pb) * n + Cr(Va + Wa X Ra - Vb - Wb X Rb) * n */
        auto relativeVelocity_v = v.vB() + XMVector3Cross(v.wB(), constraint.rB) - v.vA() - XMVector3Cross(v.wA(), constraint.rA);
        relativeVelocity_v = XMVector3Dot(relativeVelocity_v, constraint.normal);
        relativeVelocity_v = XMVectorScale(relativeVelocity_v, constraint.restitution);
        float baumgarteTerm = 0.0f;
        if constexpr(EnableBaumgarteStabilization)
        {
            baumgarteTerm = math::Max(constraint.penetrationDepth - PenetrationSlop, 0.0f) * -1.0f * BaumgarteFactor / dt;
        }

        auto baumgarte_v = XMVectorReplicate(baumgarteTerm);
        auto bias_v = XMVectorMultiply(baumgarte_v + relativeVelocity_v, g_XMIdentityR0);

        /** lagrange = -(JV + b) / (J M^-1 J^t)^-1 = effMass [-(JV + b)] */
        auto lagrange_v = MultiplyJVContact(v, constraint.jNormal, constraint.jTangent, constraint.jBitangent);
        lagrange_v = XMVectorNegate(lagrange_v + bias_v) * constraint.effectiveMass;
        Vector3 lagrange;
        XMStoreVector3(&lagrange, lagrange_v);
        auto& [lagrangeNormal, lagrangeTangent, lagrangeBitangent] = lagrange;

        /** (Ln >= 0) && (-mu*Ln <= Lt <= mu*Ln) && (-mu*Ln <= Lb < = mu*Ln) */
        float maxFriction = constraint.friction * constraint.totalLambda;
        lagrangeNormal = ClampLambda(lagrangeNormal, &constraint.totalLambda);
        lagrangeTangent = ClampMu(lagrangeTangent, maxFriction, &constraint.totalMuTangent);
        lagrangeBitangent = ClampMu(lagrangeBitangent, maxFriction, &constraint.totalMuBitangent);

        auto deltas = ComputeDeltas(constraint, lagrangeNormal, lagrangeTangent, lagrangeBitangent);

        if(constraint.physBodyA)
        {
            constraint.physBodyA->ApplyVelocities(deltas.vA(), deltas.wA());
        }

        if(constraint.physBodyB)
        {
            constraint.physBodyB->ApplyVelocities(deltas.vB(), deltas.wB());
        }
    }

    void ResolvePositionConstraint(PositionConstraint& constraint)
    {
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

    /** Compute J * V for 3 separate jacobians. Since ConstraintMatrix represents a 12 element vector,
     *  we just sum the dot product of each row. Result is returned as a vector: [Rnorm, Rtan, Rbit] */
    XMVECTOR MultiplyJVContact(const ConstraintMatrix& v,
                               const ConstraintMatrix& jNormal,
                               const ConstraintMatrix& jTangent,
                               const ConstraintMatrix& jBitangent)
    {
        auto dotVa = XMVector3Dot(jNormal.vA(), v.vA());
        auto dotWa = XMVector3Dot(jNormal.wA(), v.wA());
        auto dotVb = XMVector3Dot(jNormal.vB(), v.vB());
        auto dotWb = XMVector3Dot(jNormal.wB(), v.wB());
        auto result = dotVa + dotWa + dotVb + dotWb;

        dotVa = XMVector3Dot(jTangent.vA(), v.vA());
        dotWa = XMVector3Dot(jTangent.wA(), v.wA());
        dotVb = XMVector3Dot(jTangent.vB(), v.vB());
        dotWb = XMVector3Dot(jTangent.wB(), v.wB());
        result = XMVectorMergeXY(result, dotVa + dotWa + dotVb + dotWb);

        dotVa = XMVector3Dot(jBitangent.vA(), v.vA());
        dotWa = XMVector3Dot(jBitangent.wA(), v.wA());
        dotVb = XMVector3Dot(jBitangent.vB(), v.vB());
        dotWb = XMVector3Dot(jBitangent.wB(), v.wB());
        result = XMVectorPermute<XM_PERMUTE_0X, XM_PERMUTE_0Y, XM_PERMUTE_1X, XM_PERMUTE_1Y>(result, dotVa + dotWa + dotVb + dotWb);
        
        return result;
    }

    /** Compute velocity deltas using lagrange multipliers. */
    ConstraintMatrix ComputeDeltas(const ContactConstraint& constraint, float lNormal, float lTangent, float lBitangent)
    {
        const auto &jN = constraint.jNormal, &jT = constraint.jTangent, &jB = constraint.jBitangent;
        const auto &iA = constraint.invInertiaA, &iB = constraint.invInertiaB;
        float mA = constraint.invMassA, mB = constraint.invMassB;

        auto vA = XMVectorScale(jN.vA(), mA * lNormal) +
                  XMVectorScale(jT.vA(), mA * lTangent) +
                  XMVectorScale(jB.vA(), mA * lBitangent);

        auto wA = XMVectorScale(XMVector3Transform(jN.wA(), iA), lNormal) +
                  XMVectorScale(XMVector3Transform(jT.wA(), iA), lTangent) +
                  XMVectorScale(XMVector3Transform(jB.wA(), iA), lBitangent);

        auto vB = XMVectorScale(jN.vB(), mB * lNormal) +
                  XMVectorScale(jT.vB(), mB * lTangent) +
                  XMVectorScale(jB.vB(), mB * lBitangent);

        auto wB = XMVectorScale(XMVector3Transform(jN.wB(), iB), lNormal) +
                  XMVectorScale(XMVector3Transform(jT.wB(), iB), lTangent) +
                  XMVectorScale(XMVector3Transform(jB.wB(), iB), lBitangent);

        return ConstraintMatrix{vA, wA, vB, wB};
    }

    /** Clamping for impulse along normal. */
    float ClampLambda(float newLambda, float* totalLambda)
    {
        float temp = *totalLambda;
        *totalLambda = math::Max(0.0f, temp + newLambda);
        return *totalLambda - temp;
    }

    /** Clamping for impulse along tangents (friction). */
    float ClampMu(float newMu, float extent, float* totalMu)
    {
        float temp = *totalMu;
        *totalMu = math::Clamp(temp + newMu, -1.0f * extent, extent);
        return *totalMu - temp;
    }

    void ResolveJoint(Joint& joint)
    {
        auto vA = joint.bodyA->GetVelocity();
        auto wA = joint.bodyA->GetAngularVelocity();
        auto vB = joint.bodyB->GetVelocity();
        auto wB = joint.bodyB->GetAngularVelocity();

        auto relativeVelocity = vB + XMVector3Cross(wB, joint.rB) - vA - XMVector3Cross(wA, joint.rA);
        auto softness = XMVectorScale(joint.p, joint.softness);
        auto impulse = XMVector3Transform(joint.bias - relativeVelocity - softness, joint.m);
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
} // end anonymous namespace

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
            const Entity entityA = manifold.event.first;
            const Entity entityB = manifold.event.second;
            auto* transformA = m_registry->Get<Transform>(entityA);
            auto* transformB = m_registry->Get<Transform>(entityB);
            auto* physBodyA = m_registry->Get<PhysicsBody>(entityA);
            auto* physBodyB = m_registry->Get<PhysicsBody>(entityB);

            if constexpr(EnableDirectPositionCorrection)
            {
                auto deepestContact = manifold.GetDeepestContact();
                m_positionConstraints.emplace_back(transformA, transformB, manifold.event.eventType, deepestContact.normal, deepestContact.depth);
            }

            for(const auto& contact : manifold.contacts)
            {
                #ifdef USE_DEBUG_RENDERING
                graphics::DebugRenderer::AddPoint(contact.worldPointA);
                graphics::DebugRenderer::AddPoint(contact.worldPointB);
                #endif

                m_contactConstraints.push_back(CreateContactConstraint(contact, entityA, entityB, transformA, transformB, physBodyA, physBodyB));
            }
        }
    }

    void Solver::ResolveConstraints(std::span<Joint> joints, float dt)
    {
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
} // end namespace nc::physics