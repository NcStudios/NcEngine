#include "Solver.h"
#include "physics/PhysicsConstants.h"
#include "graphics/DebugRenderer.h"
#include "debug/Profiler.h"
//#define USE_DEBUG_RENDERING
#undef USE_DEBUG_RENDERING

#include <cassert>

using namespace DirectX;

namespace
{
    using namespace nc;
    using namespace nc::physics;

    /** Default properties for entities without a PhysicsBody. */
    const auto g_StaticPhysicsProperties = PhysicsProperties{.mass = 0.0f, .useGravity = false};
    const auto g_StaticInverseInertia = XMMATRIX{g_XMZero, g_XMZero, g_XMZero, g_XMZero};

    auto CreateContactConstraint(const Contact&, Entity, Entity, Transform*, Transform*, PhysicsBody*, PhysicsBody*) -> ContactConstraint;
    void UpdateJoint(Registry* registry, Joint& joint, float dt);
    void ResolveContactConstraint(ContactConstraint& constraint, float dt);
    void ResolvePositionConstraint(PositionConstraint& constraint);
    void ResolveJoint(Joint& joint);
    auto MultiplyJVContact(const ConstraintMatrix& v, const ConstraintMatrix& jNormal, const ConstraintMatrix& jTangent, const ConstraintMatrix& jBitangent) -> XMVECTOR;
    auto ComputeDeltas(const ContactConstraint& constraint, float lNormal, float lTangent, float lBitangent) -> ConstraintMatrix;
    auto ClampLambda(float newLambda, float* totalLambda) -> float;
    auto ClampMu(float newMu, float extent, float* totalMu) -> float;
    auto Skew(FXMVECTOR vec) -> XMMATRIX;

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

    XMMATRIX Skew(FXMVECTOR vec)
    {
        Vector3 v;
        XMStoreVector3(&v, vec);

        return XMMatrixSet
        (
            0.0f, -v.z,  v.y, 0.0f,
             v.z, 0.0f, -v.x, 0.0f,
            -v.y,  v.x, 0.0f, 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f
        );
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

    void UpdateJoint(Registry* registry, Joint& joint, float dt)
    {
        // Get anchor world space positions
        XMVECTOR pA, pB;
        {
            auto* transform = registry->Get<Transform>(joint.entityA);
            auto position = transform->GetPosition();
            auto rotation = transform->GetRotation();
            auto rotMatrix = XMMatrixRotationQuaternion(XMLoadQuaternion(&rotation));
            joint.rA = XMVector3Transform(joint.anchorA, rotMatrix);
            pA = XMLoadVector3(&position) + joint.rA;

            transform = registry->Get<Transform>(joint.entityB);
            position = transform->GetPosition();
            rotation = transform->GetRotation();
            rotMatrix = XMMatrixRotationQuaternion(XMLoadQuaternion(&rotation));
            joint.rB = XMVector3Transform(joint.anchorB, rotMatrix);
            pB = XMLoadVector3(&position) + joint.rB;
        }

        /** Scale bias by time step and positional error */
        joint.bias = -1.0f * joint.biasFactor * (1.0f / dt) * (pB - pA);

        /** Fetch inverse mass and inertia of each body */
        joint.bodyA = registry->Get<PhysicsBody>(joint.entityA);
        joint.bodyB = registry->Get<PhysicsBody>(joint.entityB);
        IF_THROW(!joint.bodyA || !joint.bodyB, "UpdateJoint - Entity does not have a PhysicsBody");
        auto invMassA = joint.bodyA->GetInverseMass();
        const auto& invInertiaA = joint.bodyA->GetInverseInertia();
        auto invMassB = joint.bodyB->GetInverseMass();
        const auto& invInertiaB = joint.bodyB->GetInverseInertia();

        /** effectiveMass = [(M^-1) - (skewRa * Ia^-1 * skewRa) - (skewRb * Ib^-1 * skewRb)]^-1 */
        {
            auto invMass = invMassA + invMassB;
            auto k1 = XMMatrixScaling(invMass, invMass, invMass);
            auto skewRa = Skew(joint.rA);
            auto k2 = skewRa * invInertiaA * skewRa; 
            auto skewRb = Skew(joint.rB);
            auto k3 = skewRb * invInertiaB * skewRb;
            joint.m = XMMatrixInverse(nullptr, k1 - k2 - k3);
        }

        /** Apply or zero out accumulated impulse */
        if constexpr(EnableJointWarmstarting)
        {
            auto vA = XMVectorScale(joint.p, invMassA * WarmstartFactor);
            vA = XMVectorNegate(XMVectorScale(vA, WarmstartFactor));
            auto wA = XMVector3Transform(XMVector3Cross(joint.rA, joint.p), invInertiaA);
            wA = XMVectorNegate(XMVectorScale(wA, WarmstartFactor));
            joint.bodyA->ApplyVelocities(vA, wA);

            auto vB = XMVectorScale(joint.p, invMassB);
            vB = XMVectorScale(vB, WarmstartFactor);
            auto wB = XMVector3Transform(XMVector3Cross(joint.rB, joint.p), invInertiaB);
            wB =  XMVectorScale(wB, WarmstartFactor);
            joint.bodyB->ApplyVelocities(vB, wB);
        }
        else
        {
            joint.p = g_XMZero;
        }
    }
} // end anonymous namespace

namespace nc::physics
{
    void ResolveConstraints(Constraints& constraints, std::span<Joint> joints, float dt)
    {
        for(size_t i = 0u; i < SolverIterations; ++i)
        {
            for(auto& constraint : constraints.contact)
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
            for(auto& constraint : constraints.position)
            {
                ResolvePositionConstraint(constraint);
            }
        }
    }

    void GenerateConstraints(Registry* registry, std::span<const Manifold> manifolds, Constraints* out)
    {
        const auto manifoldCount = manifolds.size();
        out->contact.clear();
        out->contact.reserve(manifoldCount * 4u);

        if constexpr(EnableDirectPositionCorrection)
        {
            out->position.clear();
            out->position.reserve(manifoldCount);
        }

        for(const auto& manifold : manifolds)
        {
            Entity entityA = Entity{manifold.entityA};
            Entity entityB = Entity{manifold.entityB};
            auto* transformA = registry->Get<Transform>(entityA);
            auto* transformB = registry->Get<Transform>(entityB);
            auto* physBodyA = registry->Get<PhysicsBody>(entityA);
            auto* physBodyB = registry->Get<PhysicsBody>(entityB);

            if constexpr(EnableDirectPositionCorrection)
            {
                auto deepestContact = manifold.GetDeepestContact();
                out->position.emplace_back(transformA, transformB, manifold.eventType, deepestContact.normal, deepestContact.depth);
            }

            for(const auto& contact : manifold.contacts)
            {
                #ifdef USE_DEBUG_RENDERING
                graphics::DebugRenderer::AddPoint(contact.worldPointA);
                graphics::DebugRenderer::AddPoint(contact.worldPointB);
                #endif

                out->contact.push_back(CreateContactConstraint(contact, entityA, entityB, transformA, transformB, physBodyA, physBodyB));
            }
        }
    }

    void UpdateJoints(Registry* registry, std::span<Joint> joints, float dt)
    {
        for(auto& joint : joints)
        {
            UpdateJoint(registry, joint, dt);
        }
    }

    void CacheImpulses(std::span<const ContactConstraint> constraints, std::span<Manifold> manifolds)
    {
        auto index = 0u;

        for(auto& manifold : manifolds)
        {
            for(auto& contact : manifold.contacts)
            {
                assert(index < constraints.size());
                assert(manifold.entityA == constraints[index].entityA && manifold.entityB == constraints[index].entityB);

                const auto& constraint = constraints[index];
                contact.lambda = constraint.totalLambda;
                contact.muTangent = constraint.totalMuTangent;
                contact.muBitangent = constraint.totalMuBitangent;
                ++index;
            }
        }
    }
} // end namespace nc::physics