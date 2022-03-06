#pragma once

#include "Module.h"
#include "physics/IClickable.h"

namespace nc
{
    struct PhysicsModule : public Module
    {
        /** Add and remove joints between entities. Both entities must have a PhysicsBody.
        *  Joints must be manually removed before an object is destroyed. */
        virtual void AddJoint(Entity entityA, Entity entityB, const Vector3& anchorA, const Vector3& anchorB, float bias = 0.2f, float softness = 0.0f) = 0;
        virtual void RemoveJoint(Entity entityA, Entity entityB) = 0;
        virtual void RemoveAllJoints(Entity entity) = 0;

        virtual void RegisterClickable(IClickable* clickable) = 0;
        virtual void UnregisterClickable(IClickable* clickable) noexcept = 0;
        virtual auto RaycastToClickables(LayerMask mask = LayerMaskAll) -> IClickable* = 0;
    };
}