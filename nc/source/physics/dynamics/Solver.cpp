#include "Solver.h"
#include "physics/PhysicsConstants.h"
#include "graphics/DebugRenderer.h"
#include "debug/Profiler.h"

using namespace DirectX;

namespace
{
    using namespace nc;
    using namespace nc::physics;

    const PhysicsProperties g_StaticPhysicsProperties = PhysicsProperties{.mass = 0.0f, .useGravity = false};
    const DirectX::XMMATRIX g_StaticInverseInertia = XMMATRIX{g_XMZero, g_XMZero, g_XMZero, g_XMZero};

    ContactConstraint CreateContactConstraint(const Contact&, Entity, Entity, Transform*, Transform*, PhysicsBody*, PhysicsBody*);
    void ResolveConstraint(ContactConstraint& constraint, float dt);
    XMVECTOR MultiplyJVContact(const ConstraintMatrix& v, const ConstraintMatrix& jNormal, const ConstraintMatrix& jTangent, const ConstraintMatrix& jBitangent);
    ConstraintMatrix ComputeDeltas(const ContactConstraint& constraint, float lNormal, float lTangent, float lBitangent);
    float ClampLambda(float newLambda, float* totalLambda);
    float ClampMu(float newMu, float extent, float* totalMu);

    /** Resolve contacts through sequential impulse. */
    void ResolveConstraint(ContactConstraint& constraint, float dt)
    {
        NC_PROFILE_BEGIN(debug::profiler::Filter::Dynamics);
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
        auto relativeVelocity_v = v.vA() + XMVector3Cross(v.wA(), constraint.rA) - v.vB() - XMVector3Cross(v.wB(), constraint.rB);
        relativeVelocity_v = XMVector3Dot(relativeVelocity_v, constraint.normal);
        relativeVelocity_v = XMVectorScale(relativeVelocity_v, constraint.restitution);
        float baumgarteTerm = math::Max(constraint.penetrationDepth - PenetrationSlop, 0.0f) * -1.0f * constraint.baumgarte / dt;
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
            constraint.physBodyA->UpdateVelocities(deltas.vA(), deltas.wA());
        }

        if(constraint.physBodyB)
        {
            constraint.physBodyB->UpdateVelocities(deltas.vB(), deltas.wB());
        }
        NC_PROFILE_END();
    }

    /** Compute J * V for 3 separate jacobians. Since ConstraintMatrix represents a 12 element vector,
     *  we just sum the dot product of each row. Result is returned as a vector: [Rnorm, Rtan, Rbit] */
    XMVECTOR MultiplyJVContact(const ConstraintMatrix& v,
                               const ConstraintMatrix& jNormal,
                               const ConstraintMatrix& jTangent,
                               const ConstraintMatrix& jBitangent)
    {
        NC_PROFILE_BEGIN(debug::profiler::Filter::Dynamics);
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
        
        NC_PROFILE_END();
        return result;
    }

    /** Compute velocity deltas using lagrange multipliers. */
    ConstraintMatrix ComputeDeltas(const ContactConstraint& constraint, float lNormal, float lTangent, float lBitangent)
    {
        NC_PROFILE_BEGIN(debug::profiler::Filter::Dynamics);
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

        NC_PROFILE_END();
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

    ContactConstraint CreateContactConstraint(const Contact& contact,
                                              Entity entityA,
                                              Entity entityB,
                                              Transform* transformA,
                                              Transform* transformB,
                                              PhysicsBody* physBodyA,
                                              PhysicsBody* physBodyB)
    {
        NC_PROFILE_BEGIN(debug::profiler::Filter::Dynamics);

        auto rA = contact.worldPointA - transformA->GetPosition();
        auto rB = contact.worldPointB - transformB->GetPosition();
        auto rA_v = XMLoadVector3(&rA);
        auto rB_v = XMLoadVector3(&rB);

        auto normal_v = XMLoadVector3(&contact.normal);
        auto tangent_v = XMVector3Normalize(XMVector3Orthogonal(normal_v));
        auto bitangent_v = XMVector3Normalize(XMVector3Cross(normal_v, tangent_v));

        auto jNormal = ConstraintMatrix::VelocityJacobian(rA_v, rB_v, normal_v);
        auto jTangent = ConstraintMatrix::VelocityJacobian(rA_v, rB_v, tangent_v);
        auto jBitangent = ConstraintMatrix::VelocityJacobian(rA_v, rB_v, bitangent_v);

        auto& bodyA = physBodyA ? physBodyA->GetProperties() : g_StaticPhysicsProperties;
        const auto& invInertiaA = physBodyA ? physBodyA->GetInverseInertia() : g_StaticInverseInertia;

        auto& bodyB = physBodyB ? physBodyB->GetProperties() : g_StaticPhysicsProperties;
        const auto& invInertiaB = physBodyB ? physBodyB->GetInverseInertia() : g_StaticInverseInertia;

        auto jInertiaProductA = XMVector3Transform(jNormal.wA(), invInertiaA);
        auto jInertiaProductB = XMVector3Transform(jNormal.wB(), invInertiaB);
        auto dotNormal = XMVector3Dot(jNormal.wA(), jInertiaProductA) + XMVector3Dot(jNormal.wB(), jInertiaProductB);

        jInertiaProductA = XMVector3Transform(jTangent.wA(), invInertiaA);
        jInertiaProductB = XMVector3Transform(jTangent.wB(), invInertiaB);
        auto dotTangent = XMVector3Dot(jTangent.wA(), jInertiaProductA) + XMVector3Dot(jTangent.wB(), jInertiaProductB);
        
        jInertiaProductA = XMVector3Transform(jBitangent.wA(), invInertiaA);
        jInertiaProductB = XMVector3Transform(jBitangent.wB(), invInertiaB);
        auto dotBitangent = XMVector3Dot(jBitangent.wA(), jInertiaProductA) + XMVector3Dot(jBitangent.wB(), jInertiaProductB);

        auto invMassA = bodyA.mass == 0.0f ? 0.0f : 1.0f / bodyA.mass;
        auto invMassB = bodyB.mass == 0.0f ? 0.0f : 1.0f / bodyB.mass;

        auto effectiveMass = XMVectorMergeXY(dotNormal, dotTangent);
        effectiveMass = XMVectorPermute<XM_PERMUTE_0X, XM_PERMUTE_0Y, XM_PERMUTE_1X, XM_PERMUTE_1Y>(effectiveMass, dotBitangent);
        effectiveMass = effectiveMass + XMVectorReplicate(invMassA + invMassB);
        effectiveMass = XMVectorReciprocal(effectiveMass);

        float restitution = bodyA.restitution * bodyB.restitution;
        float friction = bodyA.friction * bodyB.friction;
        float baumgarte = bodyA.baumgarte * bodyB.baumgarte;

        NC_PROFILE_END();

        return ContactConstraint
        {
            entityA, entityB,
            physBodyA, physBodyB,
            jNormal, jTangent, jBitangent,
            invInertiaA, invInertiaB,
            normal_v, rA_v, rB_v, effectiveMass,
            contact.depth,
            invMassA, invMassB,
            restitution, friction, baumgarte,
            0.0f, 0.0f, 0.0f
        };
    }
} // end anonymous namespace

namespace nc::physics
{
    void ResolveConstraints(std::span<ContactConstraint> constraints, float dt)
    {
        for(size_t i = 0u; i < SolverIterations; ++i)
        {
            for(auto& constraint : constraints)
            {
                ResolveConstraint(constraint, dt);
            }
        }
    }

    auto GenerateConstraints(registry_type* registry, std::span<const Manifold> manifolds) -> std::vector<ContactConstraint>
    {
        NC_PROFILE_BEGIN(debug::profiler::Filter::Dynamics);

        std::vector<ContactConstraint> constraints;

        for(const auto& manifold : manifolds)
        {
            Entity entityA = Entity{manifold.entityA};
            Entity entityB = Entity{manifold.entityB};
            auto* transformA = registry->Get<Transform>(entityA);
            auto* transformB = registry->Get<Transform>(entityB);
            auto* physBodyA = registry->Get<PhysicsBody>(entityA);
            auto* physBodyB = registry->Get<PhysicsBody>(entityB);

            for(const auto& contact : manifold.contacts)
            {
                #ifdef NC_DEBUG_RENDERING
                graphics::DebugRenderer::AddPoint(contact.worldPointA);
                graphics::DebugRenderer::AddPoint(contact.worldPointB);
                #endif

                constraints.push_back(CreateContactConstraint(contact, entityA, entityB, transformA, transformB, physBodyA, physBodyB));
            }
        }

        NC_PROFILE_END();
        return constraints;
    }
} // end namespace nc::physics