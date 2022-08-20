#include "Joint.h"
#include "physics/PhysicsBody.h"
#include "physics/PhysicsConstants.h"

namespace
{
auto Skew(DirectX::FXMVECTOR vec) -> DirectX::XMMATRIX
{
    nc::Vector3 v;
    DirectX::XMStoreVector3(&v, vec);

    return DirectX::XMMatrixSet
    (
        0.0f, -v.z,  v.y, 0.0f,
            v.z, 0.0f, -v.x, 0.0f,
        -v.y,  v.x, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    );
}
} // anonymous namespace

namespace nc::physics
{
JointSystem::JointSystem(Registry* registry)
    : m_registry{registry},
        m_joints{}
{
}

void JointSystem::AddJoint(Entity entityA, Entity entityB, const Vector3& anchorA, const Vector3& anchorB, float bias, float softness) noexcept
{
    m_joints.emplace_back
    (
        entityA,
        entityB,
        nullptr,
        nullptr,
        DirectX::XMLoadVector3(&anchorA),
        DirectX::XMLoadVector3(&anchorB),
        DirectX::XMMATRIX{},
        DirectX::XMVECTOR{},
        DirectX::XMVECTOR{},
        DirectX::XMVECTOR{},
        DirectX::XMVECTOR{},
        bias,
        softness
    );
}

void JointSystem::RemoveJoint(Entity entityA, Entity entityB) noexcept
{
    for(auto& joint : m_joints)
    {
        if(joint.entityA == entityA && joint.entityB == entityB)
        {
            joint = m_joints.back();
            m_joints.pop_back();
            return;
        }
    }
}

void JointSystem::RemoveAllJoints(Entity entity) noexcept
{
    auto beg = m_joints.rbegin();
    auto end = m_joints.rend();
    for(auto cur = beg; cur != end; ++cur)
    {
        if(cur->entityA == entity || cur->entityB == entity)
        {
            *cur = m_joints.back();
            m_joints.pop_back();
        }
    }
}

void JointSystem::Clear() noexcept
{
    m_joints.clear();
    m_joints.shrink_to_fit();
}

void JointSystem::UpdateJoints(float dt)
{
    for(auto& joint : m_joints)
    {
        UpdateJoint(joint, dt);
    }
}

void JointSystem::UpdateJoint(Joint& joint, float dt)
{
    using namespace DirectX;

    // Get anchor world space positions
    XMVECTOR pA, pB;
    {
        auto* transform = m_registry->Get<Transform>(joint.entityA);
        joint.rA = XMVector3Rotate(joint.anchorA, transform->RotationXM());
        pA = XMVectorAdd(joint.rA, transform->PositionXM());

        transform = m_registry->Get<Transform>(joint.entityB);
        joint.rB = XMVector3Rotate(joint.anchorB, transform->RotationXM());
        pB = XMVectorAdd(joint.rB, transform->PositionXM());
    }

    /** Scale bias by time step and positional error */
    joint.bias = XMVectorScale(XMVectorSubtract(pB, pA), -joint.biasFactor * (1.0f / dt));

    /** Fetch inverse mass and inertia of each body */
    joint.bodyA = m_registry->Get<PhysicsBody>(joint.entityA);
    joint.bodyB = m_registry->Get<PhysicsBody>(joint.entityB);
    NC_ASSERT(joint.bodyA && joint.bodyB, "UpdateJoint - Entity does not have a PhysicsBody");
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
} // namespace nc::physics
