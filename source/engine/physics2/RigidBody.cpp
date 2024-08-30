#include "ncengine/physics/RigidBody.h"
#include "ncutility/NcError.h"
#include "jolt/Conversion.h"

#include "Jolt/Jolt.h"
#include "Jolt/Physics/PhysicsSystem.h"

namespace nc::physics
{
RigidBody::~RigidBody() noexcept
{
    if (m_handle)
    {
        auto iBody = reinterpret_cast<JPH::BodyInterface*>(m_interface);
        auto apiBody = reinterpret_cast<JPH::Body*>(m_handle);
        const auto& id = apiBody->GetID();
        iBody->RemoveBody(id);
        iBody->DestroyBody(id);
    }
}

void RigidBody::Init(BodyHandle handle, BodyInterface interface)
{
    NC_ASSERT(!(m_self.IsStatic() && m_bodyType != BodyType::Static), "Static entities only support BodyType::Static");
    m_handle = handle;
    m_interface = interface;
}

void RigidBody::AddImpulse(const Vector3& impulse)
{
    auto iBody = reinterpret_cast<JPH::BodyInterface*>(m_interface);
    auto apiBody = reinterpret_cast<JPH::Body*>(m_handle);
    iBody->AddImpulse(apiBody->GetID(), JPH::Vec3{impulse.x, impulse.y, impulse.z});
}

void RigidBody::AddTorque(const Vector3& torque)
{
    auto iBody = reinterpret_cast<JPH::BodyInterface*>(m_interface);
    auto apiBody = reinterpret_cast<JPH::Body*>(m_handle);
    iBody->AddTorque(apiBody->GetID(), JPH::Vec3{torque.x, torque.y, torque.z});
}
} // namespace nc::physics
