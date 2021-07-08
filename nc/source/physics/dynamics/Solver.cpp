#include "Solver.h"
#include "physics/PhysicsConstants.h"
#include "graphics/DebugRenderer.h"

using namespace DirectX;

namespace nc::physics
{
    /** Compute J * V for 3 separate jacobians. Since ConstraintMatrix
     *  represents a 12 element vector, we just sum the dot product of each
     *  row. Result is returned as a vector: [Rnorm, Rtan, Rbit] */
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

    float ClampLambda(float newLambda, float* totalLambda)
    {
        float temp = *totalLambda;
        *totalLambda = math::Max(0.0f, temp + newLambda);
        return *totalLambda - temp;
    }

    float ClampMu(float newMu, float extent, float* totalMu)
    {
        float temp = *totalMu;
        *totalMu = math::Clamp(temp + newMu, -1.0f * extent, extent);
        return *totalMu - temp;
    }

    BasicContactConstraint CreateBasicContactConstraint(Entity a, Entity b, Transform* transformA, Transform* transformB, const Manifold& manifold)
    {
        const auto& contact = manifold.GetDeepestContact();
        return BasicContactConstraint
        {
            EntityUtils::IsStatic(a) ? nullptr : transformA,
            EntityUtils::IsStatic(b) ? nullptr : transformB,
            contact.normal * contact.depth
        };
    }

    ContactConstraint CreateContactConstraint(const Contact& contact,
                                              Entity entityA,
                                              Entity entityB,
                                              Transform* transformA,
                                              Transform* transformB,
                                              PhysicsBody* physBodyA,
                                              PhysicsBody* physBodyB)
    {
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

        auto& bodyA = physBodyA->GetProperties();
        auto& bodyB = physBodyB->GetProperties();

        const auto& invInertiaA = physBodyA->GetInverseInertia();
        const auto& invInertiaB = physBodyB->GetInverseInertia();

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

        return ContactConstraint
        {
            entityA, entityB,
            physBodyA, physBodyB,
            jNormal, jTangent, jBitangent,
            invInertiaA, invInertiaB,
            normal_v, rA_v, rB_v, effectiveMass,
            contact.depth,
            invMassA, invMassB,
            0.0f, 0.0f, 0.0f
        };
    }

    void GenerateConstraints(registry_type* registry, const std::vector<Manifold>& manifolds, Constraints* constraints)
    {
        for(const auto& manifold : manifolds)
        {
            Entity entityA = Entity{manifold.entityA};
            Entity entityB = Entity{manifold.entityB};
            auto* transformA = registry->Get<Transform>(entityA);
            auto* transformB = registry->Get<Transform>(entityB);
            auto* physBodyA = registry->Get<PhysicsBody>(entityA);
            auto* physBodyB = registry->Get<PhysicsBody>(entityB);

            if(!physBodyA || !physBodyB)
            {
                constraints->basic.push_back(CreateBasicContactConstraint(entityA, entityB, transformA, transformB, manifold));
                continue;
            }

            for(const auto& contact : manifold.contacts)
            {
                #ifdef NC_DEBUG_RENDERING
                graphics::DebugRenderer::AddPoint(contact.worldPointA);
                graphics::DebugRenderer::AddPoint(contact.worldPointB);
                #endif

                constraints->contact.push_back(CreateContactConstraint(contact, entityA, entityB, transformA, transformB, physBodyA, physBodyB));
            }
        }
    }

    void ResolveConstraint(const BasicContactConstraint& constraint)
    {
        if(constraint.transformA)
            constraint.transformA->Translate(-constraint.mtv);

        if(constraint.transformB)
            constraint.transformB->Translate(constraint.mtv);
    }

    void ResolveConstraint(ContactConstraint& constraint, float dt)
    {
        auto& bodyA = constraint.physBodyA->GetProperties();
        auto& bodyB = constraint.physBodyB->GetProperties();

        /** V = [Va, Wa, Vb, Wb] */
        ConstraintMatrix v{bodyA.velocity, bodyA.angularVelocity, bodyB.velocity, bodyB.angularVelocity};

        /** Baumgarte Stabilization / Restitution
         *  bias = b / h (Pa-Pb) * n + Cr(Va + Wa X Ra - Vb - Wb X Rb) * n */
        float restitution = bodyA.restitution * bodyB.restitution;
        auto relativeVelocity_v = v.vA() + XMVector3Cross(v.wA(), constraint.rA) - v.vB() + XMVector3Cross(v.wB(), constraint.rB);
        relativeVelocity_v = XMVector3Dot(relativeVelocity_v, constraint.normal);
        relativeVelocity_v = XMVectorScale(relativeVelocity_v, restitution);
        float baumgarteTerm = math::Max(constraint.penetrationDepth - PenetrationSlop, 0.0f) * -1.0f * bodyA.baumgarte * bodyB.baumgarte / dt;
        auto baumgarte_v = XMVectorReplicate(baumgarteTerm);
        auto bias_v = XMVectorMultiply(baumgarte_v + relativeVelocity_v, g_XMIdentityR0);

        /** lagrange = -(JV + b) / (J M^-1 J^t)^-1 = effMass [-(JV + b)] */
        auto lagrange_v = MultiplyJVContact(v, constraint.jNormal, constraint.jTangent, constraint.jBitangent);
        lagrange_v = XMVectorNegate(lagrange_v + bias_v) * constraint.effectiveMass;
        Vector3 lagrange;
        XMStoreVector3(&lagrange, lagrange_v);
        auto& [lagrangeNormal, lagrangeTangent, lagrangeBitangent] = lagrange;

        /** (Ln >= 0) && (-mu*Ln <= Lt <= mu*Ln) && (-mu*Ln <= Lb < = mu*Ln) */
        float friction = bodyA.friction * bodyB.friction;
        float maxFriction = friction * constraint.totalLambda;
        lagrangeNormal = ClampLambda(lagrangeNormal, &constraint.totalLambda);
        lagrangeTangent = ClampMu(lagrangeTangent, maxFriction, &constraint.totalMuTangent);
        lagrangeBitangent = ClampMu(lagrangeBitangent, maxFriction, &constraint.totalMuBitangent);

        auto deltas = ComputeDeltas(constraint, lagrangeNormal, lagrangeTangent, lagrangeBitangent);

        Vector3 deltaVA, deltaWA, deltaVB, deltaWB;
        XMStoreVector3(&deltaVA, deltas.vA());
        XMStoreVector3(&deltaWA, deltas.wA());
        XMStoreVector3(&deltaVB, deltas.vB());
        XMStoreVector3(&deltaWB, deltas.wB());
        bodyA.velocity += deltaVA;
        bodyA.angularVelocity += deltaWA;
        bodyB.velocity += deltaVB;
        bodyB.angularVelocity += deltaWB;
    }
}