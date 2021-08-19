#pragma once

#include "Ecs.h"
#include "physics/IClickable.h"

namespace nc::physics
{
    /** Raycast from the main camera to mouse position against IClickables in the
     *  layers specified by mask. Returns the first hit or nullptr on failure. */
    [[nodiscard]] IClickable* RaycastToClickables(LayerMask mask = LayerMaskAll);

    /** Include objects in RaycastToClickables checks. Objects must be unregistered
     *  before they are destroyed. */
    void RegisterClickable(IClickable* clickable);
    void UnregisterClickable(IClickable* clickable) noexcept;

    /** Add and remove joints between entities. Both entities must have a PhysicsBody.
     *  Joints must be manually removed before an object is destroyed. */
    void AddJoint(Entity entityA, Entity entityB, const Vector3& anchorA, const Vector3& anchorB, float bias = 0.2f, float softness = 0.0f);
    void RemoveJoint(Entity entityA, Entity entityB);
    void RemoveAllJoints(Entity entity);
}