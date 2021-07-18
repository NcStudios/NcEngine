#pragma once

#include "Manifold.h"
#include "ecs/ColliderTree.h"

#include <cstdint>
#include <vector>

namespace nc::physics
{
    /** Handling collisions requires either a physics response and collision notification
     *  or a trigger notification with no physics. */
    enum CollisionInteractionType
    {
        Physics,
        Trigger
    };

    /** An estimated bounding volume and index mapping to the associated
     *  collider's position in the SoA representation. */
    struct DynamicEstimate
    {
        SphereCollider estimate;
        uint32_t index;
        CollisionInteractionType interactionType;
    };

    /** Indices of two dynamic collider's positions in SoA.
     *  Produced by broad detection, consumed by narrow detection. */
    struct BroadDetectVsDynamicEvent
    {
        uint32_t first;
        uint32_t second;
    };

    /** Index of dynamic collider's SoA position and pointer to static
     *  collider tree entry. Produced by broad detection, consumed by
     *  narrow detection. */
    struct BroadDetectVsStaticEvent
    {
        uint32_t first;
        const ecs::StaticTreeEntry* second;
    };

    /** Entitys of two colliding objects. Produced by narrow detection,
     *  consumed by notification. */
    struct NarrowDetectEvent
    {
        Entity first;
        Entity second;
    };

    /** Collection of broad phase events. */
    struct BroadPhaseCache
    {
        std::vector<BroadDetectVsDynamicEvent> physics;
        std::vector<BroadDetectVsDynamicEvent> trigger;
        std::vector<BroadDetectVsStaticEvent> staticPhysics;
        std::vector<BroadDetectVsStaticEvent> staticTrigger;
    };

    /** Collection of narrow phase events. */
    struct NarrowPhaseCache
    {
        std::vector<NarrowDetectEvent> physics;
        std::vector<NarrowDetectEvent> trigger;
    };

    /** A container for tracking collision events and manifolds. */
    struct CollisionCache
    {
        BroadPhaseCache broad;
        NarrowPhaseCache narrow;
        NarrowPhaseCache previousNarrow;
        std::vector<DynamicEstimate> estimates;
        std::vector<Manifold> manifolds;
    };

    void UpdatePreviousEvents(CollisionCache* cache);
    void Clear(CollisionCache* cache);
    bool operator ==(const BroadDetectVsStaticEvent& lhs, const BroadDetectVsStaticEvent& rhs);
    bool operator ==(const NarrowDetectEvent& lhs, const NarrowDetectEvent& rhs);
} // namespace nc::physics