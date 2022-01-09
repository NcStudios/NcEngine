#pragma once

#include "ecs/component/Collider.h"
#include "debug/NcError.h"

#include <vector>

namespace nc { class PhysicsBody; class Registry; }

namespace nc::physics
{
    /** Specifies how to respond to a collision event. A physics response may be
     *  restricted to a single body but will always notify both entities. */
    enum class CollisionEventType : uint8_t
    {
        None,
        Trigger,
        TwoBodyPhysics,
        FirstBodyPhysics,
        SecondBodyPhysics
    };

    /** Properties describing how an object interacts with the world. */
    class ClientObjectProperties
    {
        static constexpr uint8_t Trigger   = 0b00000001;
        static constexpr uint8_t NoBody    = 0b00000010;
        static constexpr uint8_t Kinematic = 0b00000100;

        public:
            ClientObjectProperties(bool isTrigger, bool noBody, bool isKinematic);
            ClientObjectProperties(bool isTrigger, const PhysicsBody* body);

            auto EventType(ClientObjectProperties second) const -> CollisionEventType;
            auto IsTrigger() const noexcept -> bool { return m_index & Trigger; }
            auto HasPhysicsBody() const noexcept -> bool { return !(m_index & NoBody); }
            auto IsKinematic() const noexcept -> bool { return m_index & Kinematic; }

        private:
            uint8_t m_index;
    };

    /** A proxy is any object that provides access to physics-related object data. */
    template<class T>
    concept Proxy = requires(T a, T b)
    {
        { a.Matrix() } -> std::same_as<DirectX::FXMMATRIX>;
        { a.Volume() } -> std::same_as<const BoundingVolume&>;
        { a.Estimate() } -> std::same_as<const Sphere&>;
        { a.Properties() } -> std::same_as<ClientObjectProperties>;
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

    /** A persistent list of contact points for a pair. Retains at most four points,
     *  prioritizing those that maximize depth and area. */
    class Manifold
    {
        static constexpr size_t MaxPointCount = 4u;

        public:
            Manifold(Entity a, Entity b, CollisionEventType type, const Contact& contact);

            void AddContact(const Contact& contact);
            void UpdateWorldPoints(const Registry* registry);

            auto DeepestContact() const -> const Contact&;
            auto Event() const -> const NarrowEvent& { return m_event; }
            auto Event() -> NarrowEvent& { return m_event; }
            auto Contacts() const -> std::span<const Contact> { return m_contacts; }
            auto Contacts() -> std::span<Contact> { return m_contacts; }

        private:
            NarrowEvent m_event;
            std::vector<Contact> m_contacts;

            auto SortPoints(const Contact& contact) -> size_t;
    };

    template<class T>
    concept ConcavePhase = requires(T phase)
    {
        /** @todo FindPairs */
        { phase.Pairs() } -> std::same_as<const NarrowPhysicsResult&>;
        phase.Clear();
    };
}