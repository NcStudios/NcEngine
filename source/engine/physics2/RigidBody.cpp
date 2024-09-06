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

auto RigidBody::IsAwake() const -> bool
{
    return ToBody(m_handle)->IsActive();
}

void RigidBody::AddImpulse(const Vector3& impulse)
{
    m_ctx->interface.AddImpulse(ToBody(m_handle)->GetID(), ToJoltVec3(impulse));
}

void RigidBody::AddTorque(const Vector3& torque)
{
    m_ctx->interface.AddTorque(ToBody(m_handle)->GetID(), ToJoltVec3(torque));
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
    NC_ASSERT(!(m_self.IsStatic() && m_bodyType != BodyType::Static), "Static entities only support BodyType::Static");
    m_handle = handle;
    m_ctx = ctx;
}
} // namespace nc::physics
