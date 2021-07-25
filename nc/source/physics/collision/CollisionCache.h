#pragma once

#include "Manifold.h"

#include <cstdint>

namespace nc::physics
{
    /** Specifies how a Collider interacts with other colliders. */
    enum class ColliderInteractionType
    {
        Collider,               // Collider
        Physics,                // Collider and PhysicsBody
        KinematicPhysics,       // Collider and PhysicsBody with kinematic == true
        ColliderTrigger,        // Collider with isTrigger == true
        PhysicsTrigger,         // Collider with isTrigger == true and PhysicsBody
        KinematicPhysicsTrigger // Collider and PhysicsBody with both flags == true
    };

    /** Specifies whether a collision event requires no action, a physics response and
     *  collision notification, or a trigger notification. */
    enum class CollisionEventType
    {
        None, Physics, Trigger
    };

    /** A collider's estimated bounding volume, index in the registry storage, and
     *  interaction type. Used for broad detection. */
    struct ColliderEstimate
    {
        SphereCollider estimate;
        uint32_t index;
        ColliderInteractionType interactionType;
        bool isAwake;
    };

    /** Indices of two dynamic collider's positions in the registry.
     *  Produced by broad detection, consumed by narrow detection. */
    struct BroadEvent
    {
        uint32_t first;
        uint32_t second;
    };

    /** Collection of broad phase physics and trigger events. */
    struct BroadResult
    {
        std::vector<BroadEvent> physics;
        std::vector<BroadEvent> trigger;
    };

    /** Entities of two colliding objects. Produced by narrow detection,
     *  consumed by notification. */
    struct NarrowEvent
    {
        Entity first;
        Entity second;
    };

    /** Collision events and contact points for narrow phase detection.
     *  Each pair in events receives an OnCollisionXXX event. The contact
     *  points must be merged into the existing manifolds. */
    struct NarrowPhysicsResult
    {
        std::vector<NarrowEvent> events;
        std::vector<Contact> contacts;
    };

    /** A container for tracking collision events and manifolds from frame
     *  to frame. */
    struct CollisionCache
    {
        std::vector<NarrowEvent> previousPhysics;
        std::vector<NarrowEvent> previousTrigger;
        std::vector<Manifold> manifolds;
    };

    inline bool operator ==(const BroadEvent& lhs, const BroadEvent& rhs)
    {
        return (lhs.first == rhs.first && lhs.second == rhs.second);
    }

    inline bool operator ==(const NarrowEvent& lhs, const NarrowEvent& rhs)
    {
        return (lhs.first == rhs.first && lhs.second == rhs.second) ||
               (lhs.first == rhs.second && lhs.second == rhs.first);
    }
} // namespace nc::physics