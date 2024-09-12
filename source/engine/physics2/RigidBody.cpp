#include "ncengine/physics/RigidBody.h"
#include "jolt/ComponentContext.h"
#include "jolt/Conversion.h"
#include "jolt/ShapeFactory.h"

#include "Jolt/Jolt.h"
#include "Jolt/Physics/PhysicsSystem.h"
#include "ncutility/NcError.h"

namespace
{
auto ToBody(void* handle) -> JPH::Body*
{
    return reinterpret_cast<JPH::Body*>(handle);
}

auto ToActivationMode(bool wake) -> JPH::EActivation
{
    return wake ? JPH::EActivation::Activate : JPH::EActivation::DontActivate;
}
} // anonymous namespace

namespace nc::physics
{
RigidBody::~RigidBody() noexcept
{
    if (IsInitialized())
    {
        const auto& id = ToBody(m_handle)->GetID();
        m_ctx->interface.RemoveBody(id);
        m_ctx->interface.DestroyBody(id);
    }
}

void RigidBody::SetBodyType(BodyType type, bool wake)
{
    if (m_self.IsStatic())
    {
        return;
    }

    m_info.type = type;
    m_ctx->interface.SetMotionType(ToBody(m_handle)->GetID(), ToMotionType(type), ToActivationMode(wake));
}

auto RigidBody::IsAwake() const -> bool
{
    return ToBody(m_handle)->IsActive();
}

void RigidBody::SetAwakeState(bool wake)
{
    const auto id = ToBody(m_handle)->GetID();
    if (wake)
    {
        m_ctx->interface.ActivateBody(id);
    }
    else
    {
        m_ctx->interface.DeactivateBody(id);
    }
}

void RigidBody::SetShape(const Shape& shape, const Vector3& transformScale, bool wake)
{
    m_shape = shape;
    const auto allowedScaling = ScalesWithTransform()
        ? ToJoltVec3(NormalizeScaleForShape(m_shape.GetType(), transformScale, transformScale))
        : JPH::Vec3::sReplicate(1.0f);

    const auto newShape = m_ctx->shapeFactory.MakeShape(m_shape, allowedScaling);
    m_ctx->interface.SetShape(ToBody(m_handle)->GetID(), newShape, true, ToActivationMode(wake));
}

void RigidBody::SetFriction(float friction)
{
    m_info.friction = Clamp(friction, 0.0f, 1.0f);
    ToBody(m_handle)->SetFriction(m_info.friction);
}

void RigidBody::SetRestitution(float restitution)
{
    m_info.restitution = Clamp(restitution, 0.0f, 1.0f);
    ToBody(m_handle)->SetRestitution(m_info.restitution);
}

void RigidBody::SetLinearDamping(float damping)
{
    m_info.linearDamping = Clamp(damping, 0.0f, 1.0f);
    ToBody(m_handle)->GetMotionPropertiesUnchecked()->SetLinearDamping(m_info.linearDamping);
}

void RigidBody::SetAngularDamping(float damping)
{
    m_info.angularDamping = Clamp(damping, 0.0f, 1.0f);
    ToBody(m_handle)->GetMotionPropertiesUnchecked()->SetAngularDamping(m_info.angularDamping);
}

void RigidBody::SetGravityMultiplier(float factor)
{
    m_info.gravityMultiplier = Clamp(factor, 0.0f, RigidBodyInfo::maxGravityMultiplier);
    m_ctx->interface.SetGravityFactor(ToBody(m_handle)->GetID(), m_info.gravityMultiplier);
}

void RigidBody::ScalesWithTransform(bool value)
{
    m_info.flags = value
        ? m_info.flags | RigidBodyFlags::ScaleWithTransform
        : m_info.flags & ~RigidBodyFlags::ScaleWithTransform;
}

void RigidBody::UseContinuousDetection(bool value)
{
    m_ctx->interface.SetMotionQuality(ToBody(m_handle)->GetID(), ToMotionQuality(value));
    m_info.flags = value
        ? m_info.flags | RigidBodyFlags::ContinuousDetection
        : m_info.flags & ~RigidBodyFlags::ContinuousDetection;
}

auto RigidBody::GetLinearVelocity() const -> Vector3
{
    return ToVector3(ToBody(m_handle)->GetLinearVelocity());
}

void RigidBody::SetLinearVelocity(const Vector3& velocity)
{
    m_ctx->interface.SetLinearVelocity(ToBody(m_handle)->GetID(), ToJoltVec3(velocity));
}

void RigidBody::AddLinearVelocity(const Vector3& velocity)
{
    m_ctx->interface.AddLinearVelocity(ToBody(m_handle)->GetID(), ToJoltVec3(velocity));
}

auto RigidBody::GetAngularVelocity() const -> Vector3
{
    return ToVector3(ToBody(m_handle)->GetAngularVelocity());
}

void RigidBody::SetAngularVelocity(const Vector3& velocity)
{
    m_ctx->interface.SetAngularVelocity(ToBody(m_handle)->GetID(), ToJoltVec3(velocity));
}

void RigidBody::SetVelocities(const Vector3& linearVelocity, const Vector3& angularVelocity)
{
    m_ctx->interface.SetLinearAndAngularVelocity(ToBody(m_handle)->GetID(), ToJoltVec3(linearVelocity), ToJoltVec3(angularVelocity));
}

void RigidBody::AddVelocities(const Vector3& linearVelocity, const Vector3& angularVelocity)
{
    m_ctx->interface.AddLinearAndAngularVelocity(ToBody(m_handle)->GetID(), ToJoltVec3(linearVelocity), ToJoltVec3(angularVelocity));
}

void RigidBody::AddForce(const Vector3& force)
{
    m_ctx->interface.AddForce(ToBody(m_handle)->GetID(), ToJoltVec3(force));
}

void RigidBody::AddForceAt(const Vector3& force, const Vector3& point)
{
    m_ctx->interface.AddForce(ToBody(m_handle)->GetID(), ToJoltVec3(force), ToJoltVec3(point));
}

void RigidBody::AddTorque(const Vector3& torque)
{
    m_ctx->interface.AddTorque(ToBody(m_handle)->GetID(), ToJoltVec3(torque));
}

void RigidBody::AddImpulse(const Vector3& impulse)
{
    m_ctx->interface.AddImpulse(ToBody(m_handle)->GetID(), ToJoltVec3(impulse));
}

void RigidBody::AddImpulseAt(const Vector3& impulse, const Vector3& point)
{
    m_ctx->interface.AddImpulse(ToBody(m_handle)->GetID(), ToJoltVec3(impulse), ToJoltVec3(point));
}

void RigidBody::AddAngularImpulse(const Vector3& impulse)
{
    m_ctx->interface.AddAngularImpulse(ToBody(m_handle)->GetID(), ToJoltVec3(impulse));
}

void RigidBody::SetSimulatedBodyPosition(Transform& transform,
                                         const Vector3& position,
                                         bool wake)
{
    transform.SetPosition(position);
    m_ctx->interface.SetPosition(ToBody(m_handle)->GetID(), ToJoltVec3(position), ToActivationMode(wake));
}

void RigidBody::SetSimulatedBodyRotation(Transform& transform,
                                         const Quaternion& rotation,
                                         bool wake)
{
    transform.SetRotation(rotation);
    m_ctx->interface.SetRotation(ToBody(m_handle)->GetID(), ToJoltQuaternion(rotation), ToActivationMode(wake));
}

auto RigidBody::SetSimulatedBodyScale(Transform& transform,
                                      const Vector3& scale,
                                      bool wake) -> Vector3
{
    auto appliedScale = scale;
    if (ScalesWithTransform())
    {
        appliedScale = NormalizeScaleForShape(m_shape.GetType(), transform.Scale(), scale);
        const auto newShape = m_ctx->shapeFactory.MakeShape(m_shape, ToJoltVec3(appliedScale));
        m_ctx->interface.SetShape(ToBody(m_handle)->GetID(), newShape, true, ToActivationMode(wake));
    }

    transform.SetScale(appliedScale);
    return appliedScale;
}

void RigidBody::SetContext(BodyHandle handle, ComponentContext* ctx)
{
    m_handle = handle;
    m_ctx = ctx;
}
} // namespace nc::physics
