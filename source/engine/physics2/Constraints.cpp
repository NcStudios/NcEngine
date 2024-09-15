#include "ncengine/physics/Constraints.h"
#include "ncengine/physics/RigidBody.h"
#include "jolt/ConstraintManager.h"

#include "Jolt/Jolt.h"
#include "Jolt/Physics/Body/Body.h"

namespace nc::physics
{
void Constraint::Enable(bool enabled)
{
    s_manager->EnableConstraint(*this, enabled);
}

void Constraint::NotifyUpdateInfo()
{
    s_manager->UpdateConstraint(*this);
}

void Constraint::SetConstraintTarget(RigidBody* otherBody)
{
    auto referencedEntity = Entity::Null();
    auto referencedBody = &JPH::Body::sFixedToWorld;
    if (otherBody)
    {
        referencedEntity = otherBody->GetEntity();
        referencedBody = static_cast<JPH::Body*>(otherBody->GetHandle());
    }

    s_manager->UpdateConstraintTarget(*this, referencedEntity, referencedBody);
}
} // namespace nc::physics
