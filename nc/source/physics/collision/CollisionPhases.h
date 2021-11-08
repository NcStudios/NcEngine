#pragma once

#include "CollisionCache.h"


#include "physics/PhysicsPipelineTypes.h"

#include "proxy/GenericProxy.h"

namespace nc::physics
{
    auto SimplePhysics(Registry* registry) -> NarrowPhysicsResult;



    /** Compute estimates for each collider. The world space matrices are also returned,
     *  in a separate vector, for narrow phase use. */
    void FetchEstimates(const Registry* registry, CollisionStepInitData* out);

    /** Transform contact points to updated worldspace positions and remove stale points from manifolds.
     *  Remove any manifolds with no remaining contact points. */
    void UpdateManifolds(const Registry* registry, std::vector<Manifold>& manifolds);

    /** Add new contact points from narrow phase to the existing manifolds. */
    void MergeNewContacts(const NarrowPhysicsResult& newEvents, std::vector<Manifold>& manifolds);

    /** Broad phase comparison between collider estimates. Returns potential physics and trigger events. */
    void FindBroadPairs(std::span<const ColliderEstimate> estimates, size_t physicsReserveCount, size_t triggerReserveCount, BroadResult* out);

    /** Narrow phase collision detection and contact generation for collisions requiring a physics response. */
    void FindNarrowPhysicsPairs(Registry* registry, std::span<const DirectX::XMMATRIX> matrices, std::span<const BroadEvent> broadPhysicsEvents, NarrowPhysicsResult* out);

    /** Narrow phase collision detection for trigger events. */
    void FindNarrowTriggerPairs(const Registry* registry, std::span<const DirectX::XMMATRIX> matrices, std::span<const BroadEvent> broadTriggerEvents, std::vector<NarrowEvent>* out);


    void FindNarrowPhysicsPairs(Registry* registry, std::span<const BroadPair<GenericProxy>> physicsPairs, NarrowPhysicsResult* out);

    void FindNarrowTriggerPairs(std::span<const BroadPair<GenericProxy>> triggerPairs, std::vector<NarrowEvent>* out);
}