#pragma once

#include "IClickable.h"
#include "module/Module.h"

namespace nc::physics
{
/** @brief Interface for general physics functionality. */
struct PhysicsModule : public Module
{
    /**
     * @brief A a joint between two entities.
     * 
     * Both entities must have a PhysicsBody. Joints must be explicitly removed with
     * RemoveJoint() or RemoveAllJoints().
     * 
     * @param entityA The first body.
     * @param entityB The second body.
     * @param anchorA The offset from the first body's center where the joint is attached.
     * @param anchorB The offset from the second body's center where the joint is attached.
     * @param bias A float in the range [0, 1] for position stabilization.
     * @param softness A float in the range [0, 1] for softening the final impulse response.
     */
    virtual void AddJoint(Entity entityA, Entity entityB, const Vector3& anchorA, const Vector3& anchorB, float bias = 0.2f, float softness = 0.0f) = 0;

    /**
     * @brief Remove a joint between two entities.
     * @param entityA The first body.
     * @param entityB The second body.
     */
    virtual void RemoveJoint(Entity entityA, Entity entityB) = 0;

    /**
     * @brief Remove all joints attached to an Entity.
     * @param entity The body whose joints will be cleared.
     */
    virtual void RemoveAllJoints(Entity entity) = 0;

    /** @todo The whole system behind these is broken. */
    virtual void RegisterClickable(IClickable* clickable) = 0;
    virtual void UnregisterClickable(IClickable* clickable) noexcept = 0;
    virtual auto RaycastToClickables(LayerMask mask = LayerMaskAll) -> IClickable* = 0;
};
} // namespace nc::physics
