#pragma once

#include "Manifold.h"
#include "../dynamics/Constraint.h"

#include <cstdint>

namespace nc::physics
{
    /** A collider's estimated bounding volume, index in the registry storage, and
     *  interaction type. Used for broad detection. */
    struct ColliderEstimate
    {
        Sphere estimate;
        uint32_t index;
        ColliderInteractionType interactionType;
        bool isAwake;
    };

    /** A copy of world space matrices and estimates for each collider. Created at
     *  the start of the collision step. */
    struct CollisionStepInitData
    {
        std::vector<DirectX::XMMATRIX> matrices;
        std::vector<ColliderEstimate> estimates;
    };

    /** Indices of two dynamic collider's positions in the registry.
     *  Produced by broad detection, consumed by narrow detection. */
    struct BroadEvent
    {
        uint32_t first;
        uint32_t second;
        CollisionEventType eventType;
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
        CollisionEventType eventType;
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
        CollisionStepInitData initData;
        BroadResult broadResults;
        NarrowPhysicsResult physicsResults;
        NarrowPhysicsResult staticResults;
        std::vector<NarrowEvent> currentTrigger;
        std::vector<NarrowEvent> previousPhysics;
        std::vector<NarrowEvent> previousTrigger;
        std::vector<Manifold> manifolds;
        Constraints constraints;
        size_t previousBroadPhysicsCount;
        size_t previousBroadTriggerCount;
        double fixedTimeStep;
    };

    inline void UpdateCache(CollisionCache* cache)
    {
        cache->previousPhysics = std::move(cache->physicsResults.events);
        cache->previousTrigger = std::move(cache->currentTrigger);
        cache->previousBroadPhysicsCount = cache->broadResults.physics.size();
        cache->previousBroadTriggerCount = cache->broadResults.trigger.size();
    }

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