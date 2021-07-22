#pragma once

#include "CollisionCache.h"
#include "../PhysicsSystem.h"

namespace nc::physics
{
    struct PerFrameCollisionData
    {
        std::vector<DirectX::XMMATRIX> matrices;
        std::vector<ColliderEstimate> estimates;
    };

    /** Get transformation matrices collider estimates for each collider. */
    auto FetchEstimates(registry_type* registry) -> PerFrameCollisionData;

    /** Transform contact points to updated worldspace positions and remove stale points from manifolds.
     *  Remove any manifolds with no remaining contact points. */
    void UpdateManifolds(registry_type* registry, std::vector<Manifold>& manifolds);

    /** Add new contact points from narrow phase to the existing manifolds. */
    void MergeNewContacts(const NarrowPhysicsResult& newEvents, std::vector<Manifold>& manifolds);

    /** Broad phase comparison between collider estimates. Returns potential physics and trigger events. */
    auto FindBroadPairs(std::span<const ColliderEstimate> estimates) -> BroadResult;

    /** Narrow phase collision detection and contact generation for collisions requiring a physics response. */
    auto FindNarrowPhysicsPairs(std::span<Collider> colliders, std::span<const DirectX::XMMATRIX> matrices, std::span<const BroadEvent> broadPhysicsEvents) -> NarrowPhysicsResult;

    /** Narrow phase collision detection for trigger events. */
    auto FindNarrowTriggerPairs(std::span<Collider> colliders, std::span<const DirectX::XMMATRIX> matrices, std::span<const BroadEvent> broadTriggerEvents) -> std::vector<NarrowEvent>;
}