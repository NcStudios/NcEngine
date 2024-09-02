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
void SetSimulatedBodyPosition(Transform& transform,
                              RigidBody& rigidBody,
                              const Vector3& position,
                              bool wake)
{
    transform.SetPosition(position);
    rigidBody.SetPosition(position, wake);
}

void SetSimulatedBodyRotation(Transform& transform,
                              RigidBody& rigidBody,
                              const Quaternion& rotation,
                              bool wake)
{
    transform.SetRotation(rotation);
    rigidBody.SetRotation(rotation, wake);
}

auto SetSimulatedBodyScale(Transform& transform,
                           RigidBody& rigidBody,
                           const Vector3& scale,
                           bool wake) -> Vector3
{
    const auto appliedScale = rigidBody.ScalesWithTransform()
        ? rigidBody.SetScale(scale, wake)
        : scale;

    transform.SetScale(appliedScale);
    return appliedScale;
}

RigidBody::~RigidBody() noexcept
{
    if (IsInitialized())
    {
        const auto& id = ToBody(m_handle)->GetID();
        m_ctx->interface.RemoveBody(id);
        m_ctx->interface.DestroyBody(id);
    }
}

void RigidBody::AddImpulse(const Vector3& impulse)
{
    m_ctx->interface.AddImpulse(ToBody(m_handle)->GetID(), ToJoltVec3(impulse));
}

void RigidBody::AddTorque(const Vector3& torque)
{
    m_ctx->interface.AddTorque(ToBody(m_handle)->GetID(), ToJoltVec3(torque));
}

void RigidBody::SetPosition(const Vector3& position, bool wake)
{
    m_ctx->interface.SetPosition(ToBody(m_handle)->GetID(), ToJoltVec3(position), ToActivationMode(wake));
}

void RigidBody::SetRotation(const Quaternion& rotation, bool wake)
{
    m_ctx->interface.SetRotation(ToBody(m_handle)->GetID(), ToJoltQuaternion(rotation), ToActivationMode(wake));
}

auto RigidBody::SetScale(const Vector3& scale, bool wake) -> Vector3
{
    auto shapeScale = ToJoltVec3(scale);
    NormalizeScaleForShape(m_shape, shapeScale);
    auto newShape = m_ctx->shapeFactory.MakeShape(m_shape, shapeScale);
    m_ctx->interface.SetShape(ToBody(m_handle)->GetID(), newShape, true, ToActivationMode(wake));
    return ToVector3(shapeScale);
}

void RigidBody::SetContext(BodyHandle handle, ComponentContext* ctx)
{
    NC_ASSERT(!(m_self.IsStatic() && m_bodyType != BodyType::Static), "Static entities only support BodyType::Static");
    m_handle = handle;
    m_ctx = ctx;
}
} // namespace nc::physics
