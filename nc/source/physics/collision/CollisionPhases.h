#pragma once

#include "CollisionCache.h"
#include "ecs/ColliderSystem.h"

namespace nc::physics
{
    /** Update transformation matrices in the collider SoA and compute collider estimates for
     *  each dynamic collider. */
    void FetchEstimates(registry_type* registry,
                        ecs::ColliderSystem::DynamicColliderSoA* soa,
                        std::vector<DynamicEstimate>& estimates);

    /** Transform contact points to updated worldspace positions and remove stale points from
     *  manifolds. */
    void UpdateManifolds(registry_type* registry, std::vector<Manifold>& manifolds);

    /** Broad phase comparison between dynamic collider estimates. Potential physics and trigger
     *  events are output to broadPhysicsEvents and broadTrigger events, respectively. */
    void FindBroadPairs(const std::vector<DynamicEstimate>& estimates,
                        std::vector<BroadDetectVsDynamicEvent>& broadPhysicsEvents,
                        std::vector<BroadDetectVsDynamicEvent>& broadTriggerEvents);

    /** Broad phase comparison between dynamic and static collider estimates. Potential physics and
     *  trigger events are output to broadStaticPhysicsEvents and broadStaticTriggerEvents, respectively. */
    void FindBroadStaticPairs(const std::vector<DynamicEstimate>& estimates,
                              const ecs::ColliderTree* staticTree,
                              std::vector<BroadDetectVsStaticEvent>& broadStaticPhysicsEvents,
                              std::vector<BroadDetectVsStaticEvent>& broadStaticTriggerEvents);

    /** Narrow phase collision detection and contact generation for dynamic and static colliders. Events
     *  requiring physics resolution and notification are output to narrowPhysicsEvents. A new manifold or
     *  contact point will be created for each event, depending on if the event is a new or persisting. */
    void FindNarrowPhysicsPairs(const std::vector<BroadDetectVsDynamicEvent>& broadPhysicsEvents,
                                const std::vector<BroadDetectVsStaticEvent>& broadStaticPhysicsEvents,
                                const ecs::ColliderSystem::DynamicColliderSoA* soa,
                                std::vector<NarrowDetectEvent>& narrowPhysicsEvents,
                                std::vector<Manifold>& manifolds);

    /** Narrow phase trigger detection for dynamic and static colliders. Events requiring notification
     *  are output to narrowTriggerEvents. */
    void FindNarrowTriggerPairs(const std::vector<BroadDetectVsDynamicEvent>& broadTriggerEvents,
                                const std::vector<BroadDetectVsStaticEvent>& broadStaticTriggerEvents,
                                const ecs::ColliderSystem::DynamicColliderSoA* soa,
                                std::vector<NarrowDetectEvent>& narrowTriggerEvents);
}