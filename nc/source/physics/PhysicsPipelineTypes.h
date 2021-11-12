#pragma once

#include "ecs/component/Collider.h"
#include "debug/NcError.h"

#include <vector>

namespace nc { class PhysicsBody; class Registry; }

namespace nc::physics
{
    /** Specifies how a Collider interacts with other colliders. */
    enum class ColliderInteractionType
    {
        Collider,               // Default Collider
        Physics,                // Collider and PhysicsBody
        KinematicPhysics,       // Collider and PhysicsBody with isKinematic == true
        ColliderTrigger,        // Collider with isTrigger == true
        PhysicsTrigger,         // Collider with isTrigger == true and PhysicsBody
        KinematicPhysicsTrigger // Collider and PhysicsBody with both flags == true
    };

    /** Specifies how to respond to a collision event. A physics response may be
     *  restricted to a single body but will always notify both entities. */
    enum class CollisionEventType
    {
        None,
        Trigger,
        TwoBodyPhysics,
        FirstBodyPhysics,
        SecondBodyPhysics
    };

    auto GetColliderInteractionType(bool isTrigger, const PhysicsBody* body) -> ColliderInteractionType;
    auto GetEventType(ColliderInteractionType typeA, ColliderInteractionType typeB) -> CollisionEventType;

    /** A proxy is any object that provides access to physics-related object data. */
    template<class T>
    concept Proxy = requires(T a, T b)
    {
        { a.Matrix() } -> std::same_as<DirectX::FXMMATRIX>;
        { a.Volume() } -> std::same_as<const BoundingVolume&>;
        { a.Estimate() } -> std::same_as<const Sphere&>;
        { a.InteractionType() } -> std::same_as<ColliderInteractionType>;
        { a.Id() } -> std::equality_comparable;
    };

    template<class T>
    concept ProxyCache = requires(T cache)
    {
        cache.Update();
        cache.Proxies();
        /** @todo will need a few more for working with frame deltas */
    };

    /** Two proxies with overlapping estimates. */
    template<Proxy ProxyType>
    struct BroadPair
    {
        ProxyType* a;
        ProxyType* b;
        CollisionEventType eventType;
    };

    /** A collection of potential physics and trigger events. */
    template<Proxy ProxyType>
    struct BroadResults
    {
        using pair_type = BroadPair<ProxyType>;
        std::vector<pair_type> physicsPairs;
        std::vector<pair_type> triggerPairs;
    };

    /** A broad phase reads proxies from a proxy cache and filters pairs
     *  before the narrow phase is run. */
    template<class T>
    concept BroadPhase = requires(T phase, T::proxy_cache_type* cache)
    {
        phase.Update(cache);
        phase.FindPairs();
        phase.Clear();
        { phase.PhysicsPairs() } -> std::same_as<std::span<const BroadPair<typename T::proxy_type>>>;
        { phase.TriggerPairs() } -> std::same_as<std::span<const BroadPair<typename T::proxy_type>>>;
    };

    /** A pair of colliding objects. */
    struct NarrowEvent
    {
        enum class State : uint8_t { New, Persisting, Stale };

        Entity first;
        Entity second;
        CollisionEventType eventType;
        State state = State::New;
    };

    /** Contact data for a pair of colliding objects. */
    struct Contact
    {
        Vector3 worldPointA;
        Vector3 worldPointB;
        Vector3 localPointA;
        Vector3 localPointB;
        Vector3 normal;
        float depth;
        float lambda = 0.0f;
        float muTangent = 0.0f;
        float muBitangent = 0.0f;
    };

    /** Collection of physics events and contacts points. */
    struct NarrowPhysicsResult
    {
        std::vector<NarrowEvent> events = {};
        std::vector<Contact> contacts = {};
    };

    /** A persistent list of contact points for a pair. */
    struct Manifold
    {
        NarrowEvent event;
        std::vector<Contact> contacts;

        int AddContact(const Contact& contact);
        int SortPoints(const Contact& contact);
        void UpdateWorldPoints(const Registry* registry);
        const Contact& DeepestContact() const;
    };

    template<class T>
    concept ConcavePhase = requires(T phase)
    {
        /** @todo FindPairs */
        { phase.Pairs() } -> std::same_as<const NarrowPhysicsResult&>;
        phase.Clear();
    };
}