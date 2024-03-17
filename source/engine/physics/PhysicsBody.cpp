#include "physics/PhysicsBody.h"
#include "physics/Collider.h"
#include "ecs/Transform.h"
#include "ecs/Registry.h"
#include "physics/PhysicsConstants.h"
#include "physics/PhysicsDebugging.h"

using namespace DirectX;

namespace
{
using namespace nc;
using namespace nc::physics;

const auto VelocitySleepThreshold = XMVectorSet(physics::SleepEpsilon, physics::SleepEpsilon, physics::SleepEpsilon, 0.0f);
const auto AngularVelocityMin = XMVectorReplicate(-1.0f * physics::MaxAngularVelocity);
const auto AngularVelocityMax = XMVectorReplicate(physics::MaxAngularVelocity);

Vector3 CreateInverseInertiaTensor(const Transform& transform, const Collider& collider, float mass)
{
    auto transformScale = transform.Scale();
    auto colliderScale = collider.GetInfo().scale;
    auto scale = HadamardProduct(transformScale, colliderScale);
    auto type = collider.GetType();
    float iX, iY, iZ;

    switch(type)
    {
        case ColliderType::Box:
        {
            auto m = mass / 12.0f;
            auto squareScale = HadamardProduct(scale, scale);
            iX = m * (squareScale.y + squareScale.z);
            iY = m * (squareScale.x + squareScale.z);
            iZ = m * (squareScale.x + squareScale.y);
            break;
        }
        case ColliderType::Capsule:
        {
            /** Estimated with a cylinder */
            auto r = scale.x * 0.5f;
            auto h = scale.y * 2.0f;
            iX = iY = (mass / 12.0f) * (3.0f * r * r + h * h);
            iZ = mass * r * r * 0.5f;
            break;
        }
        case ColliderType::Sphere:
        {
            float radius = scale.x * 0.5f;
            iX = iY = iZ = (2.0f / 5.0f) * mass * radius * radius;
            break;
        }
        case ColliderType::Hull:
        {
            /** Estimated with a bounding box */
            const auto& hull = std::get<ConvexHull>(collider.GetVolume());
            scale = HadamardProduct(scale, hull.extents);
            auto squareScale = HadamardProduct(scale, scale);
            auto m = mass / 12.0f;
            iX = m * (squareScale.y + squareScale.z);
            iY = m * (squareScale.x + squareScale.z);
            iZ = m * (squareScale.x + squareScale.y);
            break;
        }
        default:
        {
            throw NcError("Unknown ColliderType");
        }
    }

    return Vector3{1.0f / iX, 1.0f / iY, 1.0f / iZ};
}
} // anonymous namespace

namespace nc::physics
{
PhysicsBody::PhysicsBody(const Transform& transform,
                         const Collider& collider,
                         PhysicsProperties properties)
    : m_linearVelocity{},
      m_angularVelocity{},
      m_invInertiaWorld{},
      m_invInertiaLocal{},
      m_properties{properties},
      m_framesAtThreshold{0u},
      m_awake{true}
{
    if(m_properties.mass == 0.0f)
    {
        m_invInertiaLocal = Vector3::Zero();
        return;
    }

    m_invInertiaLocal = CreateInverseInertiaTensor(transform, collider, m_properties.mass);
    m_properties.mass = 1.0f / m_properties.mass;
}

void PhysicsBody::SetProperties(const PhysicsProperties& properties)
{
    m_properties = properties;
}

void PhysicsBody::ApplyImpulse(const Vector3& impulse)
{
    ApplyImpulse(XMLoadVector3(&impulse));
}

void PhysicsBody::ApplyImpulse(DirectX::FXMVECTOR impulse)
{
    NC_PHYSICS_ASSERT(!m_properties.isKinematic, "Attempting to move immovable object");
    m_linearVelocity = XMVectorAdd(m_linearVelocity, XMVectorScale(impulse, m_properties.mass));
}

void PhysicsBody::ApplyTorqueImpulse(const Vector3& torque)
{
    ApplyTorqueImpulse(DirectX::XMLoadVector3(&torque));
}

void PhysicsBody::ApplyTorqueImpulse(DirectX::FXMVECTOR torque)
{
    NC_PHYSICS_ASSERT(!m_properties.isKinematic, "Attempting to move immovable object");
    m_angularVelocity = XMVectorAdd(m_angularVelocity, XMVector3Transform(torque, m_invInertiaWorld));
}

void PhysicsBody::ApplyVelocity(DirectX::FXMVECTOR delta)
{
    NC_PHYSICS_ASSERT(!m_properties.isKinematic, "Attempting to move immovable object");
    m_linearVelocity = XMVectorAdd(m_linearVelocity, delta);
}

void PhysicsBody::ApplyVelocities(DirectX::FXMVECTOR velDelta, DirectX::FXMVECTOR angVelDelta)
{
    NC_PHYSICS_ASSERT(!m_properties.isKinematic, "Attempting to move immovable object");
    m_linearVelocity = XMVectorAdd(m_linearVelocity, velDelta);
    m_angularVelocity = XMVectorAdd(m_angularVelocity, angVelDelta);
}

void PhysicsBody::UpdateWorldInertia(const Transform* transform)
{
    auto rot_v = transform->RotationXM();
    auto rot_m = DirectX::XMMatrixRotationQuaternion(rot_v);
    auto invInertiaLocalMatrix = DirectX::XMMatrixScaling(m_invInertiaLocal.x, m_invInertiaLocal.y, m_invInertiaLocal.z);
    m_invInertiaWorld = rot_m * invInertiaLocalMatrix * DirectX::XMMatrixTranspose(rot_m);
}

IntegrationResult PhysicsBody::Integrate(Transform* transform, float dt)
{
    using namespace DirectX;

    /** @todo Applying a force/velocity to a sleeping body will not wake it. That should
     *  probably happen here. Note that it cannot happen in ApplyXXX because the collider
     *  needs to be notified as well. */
    if(m_properties.isKinematic || !m_awake)
        return IntegrationResult::Ignored;

    auto linearDragFactor = pow(1.0f - m_properties.drag, dt);
    auto angularDragFactor = pow(1.0f - m_properties.angularDrag, dt);
    m_linearVelocity = XMVectorScale(m_linearVelocity, linearDragFactor);
    m_angularVelocity = XMVectorScale(m_angularVelocity, angularDragFactor);
    m_angularVelocity = XMVectorClamp(m_angularVelocity, AngularVelocityMin, AngularVelocityMax);

    /** @todo #634 Remove when we figure out who's seting W component*/
    m_linearVelocity = XMVectorSetW(m_linearVelocity, 0.0f);
    m_angularVelocity = XMVectorSetW(m_angularVelocity, 0.0f);

    if constexpr(physics::EnableSleeping)
    {
        auto velSquareMag = XMVectorAdd(XMVector3LengthSq(m_linearVelocity), XMVector3LengthSq(m_angularVelocity));

        if(XMVector3Less(velSquareMag, VelocitySleepThreshold))
        {
            if(++m_framesAtThreshold >= physics::FramesUntilSleep)
            {
                m_linearVelocity = g_XMZero;
                m_angularVelocity = g_XMZero;
                m_awake = false;
                return IntegrationResult::PutToSleep;
            }
        }
        else
        {
            m_framesAtThreshold = 0u;
        }
    }

    transform->Translate(XMVectorScale(m_linearVelocity, dt));
    auto rotQuat = XMVectorScale(m_angularVelocity, dt);
    rotQuat = XMQuaternionRotationRollPitchYawFromVector(rotQuat);
    transform->Rotate(rotQuat);
    return IntegrationResult::Integrated;
}
} // namespace nc::physics
