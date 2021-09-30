#pragma once

#include "CollisionCache.h"
#include "../PhysicsSystem.h"

namespace nc::physics
{
    /** Compute estimates for each collider. The world space matrices are also returned,
     *  in a separate vector, for narrow phase use. */
    void FetchEstimates(const registry_type* registry, CollisionStepInitData* out);

    /** Transform contact points to updated worldspace positions and remove stale points from manifolds.
     *  Remove any manifolds with no remaining contact points. */
    void UpdateManifolds(const registry_type* registry, std::vector<Manifold>& manifolds);

    /** Add new contact points from narrow phase to the existing manifolds. */
    void MergeNewContacts(const NarrowPhysicsResult& newEvents, std::vector<Manifold>& manifolds);

    /** Broad phase comparison between collider estimates. Returns potential physics and trigger events. */
    void FindBroadPairs(std::span<const ColliderEstimate> estimates, size_t physicsReserveCount, size_t triggerReserveCount, BroadResult* out);

    /** Narrow phase collision detection and contact generation for collisions requiring a physics response. */
    void FindNarrowPhysicsPairs(registry_type* registry, std::span<const DirectX::XMMATRIX> matrices, std::span<const BroadEvent> broadPhysicsEvents, NarrowPhysicsResult* out);

    /** Narrow phase collision detection for trigger events. */
    void FindNarrowTriggerPairs(const registry_type* registry, std::span<const DirectX::XMMATRIX> matrices, std::span<const BroadEvent> broadTriggerEvents, std::vector<NarrowEvent>* out);
}