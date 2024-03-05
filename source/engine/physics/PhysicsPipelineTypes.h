#pragma once

#include "physics/Collider.h"
#include "ncutility/NcError.h"

#include <vector>

namespace nc
{
class Registry;

namespace physics
{
class PhysicsBody;

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
        constexpr explicit ClientObjectProperties(bool isTrigger) noexcept
            : m_flags{NoBody}
        {
            if (isTrigger) m_flags |= Trigger;
        }

        constexpr explicit ClientObjectProperties(bool isTrigger, bool isKinematic) noexcept
            : m_flags{0}
        {
            if (isTrigger) m_flags |= Trigger;
            if (isKinematic) m_flags |= Kinematic;
        }

        auto EventType(ClientObjectProperties second) const -> CollisionEventType;
        auto IsTrigger() const noexcept -> bool { return m_flags & Trigger; }
        auto HasPhysicsBody() const noexcept -> bool { return !(m_flags & NoBody); }
        auto IsKinematic() const noexcept -> bool { return m_flags & Kinematic; }

    private:
        uint8_t m_flags;
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
concept BroadPhase = requires(T phase, typename T::proxy_cache_type* cache)
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
    bool stickyContacts = false;
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

/**
 * A persistent list of contact points for a pair. Retains at most four points,
 * prioritizing those that maximize depth and area. Implementation adapted from
 * Bullet Physics.
 */
class Manifold
{
    static constexpr size_t MaxPointCount = 4u;

    public:
        explicit Manifold(Entity a, Entity b, CollisionEventType type, bool stickyContacts, const Contact& contact) noexcept
            : m_event{a, b, type, stickyContacts, NarrowEvent::State::New}, m_contacts{contact} {}

        void AddContact(const Contact& contact);
        void UpdateWorldPoints(const Registry* registry);

        auto DeepestContact() const -> const Contact&;
        auto Event() const noexcept -> const NarrowEvent& { return m_event; }
        auto Event() noexcept -> NarrowEvent& { return m_event; }
        auto Contacts() const noexcept -> std::span<const Contact> { return m_contacts; }
        auto Contacts() noexcept -> std::span<Contact> { return m_contacts; }

    private:
        NarrowEvent m_event;
        std::vector<Contact> m_contacts;

        auto SortPoints(const Contact& contact) -> size_t;
};

inline auto UseStickyContacts(Entity entityA, Entity entityB, const BoundingVolume& bva, const BoundingVolume& bvb) -> bool
{
    if (!entityA.IsStatic() && !entityB.IsStatic())
        return false;

    return std::visit([](auto&& a, auto&& b)
    {
        using bva_t = std::decay_t<decltype(a)>;
        using bvb_t = std::decay_t<decltype(b)>;

        if constexpr (std::same_as<bva_t, Box> && std::same_as<bvb_t, Box>)
            return true;
        else if constexpr (std::same_as<bva_t, Box> && std::same_as<bvb_t, ConvexHull>)
            return true;
        else if constexpr (std::same_as<bva_t, ConvexHull> && std::same_as<bvb_t, Box>)
            return true;
        else if constexpr (std::same_as<bva_t, ConvexHull> && std::same_as<bvb_t, ConvexHull>)
            return true;
        else
            return false;
    }, bva, bvb);
}

template<class T>
concept ConcavePhase = requires(T phase)
{
    /** @todo FindPairs */
    { phase.Pairs() } -> std::same_as<const NarrowPhysicsResult&>;
    phase.Clear();
};
} // namespace physics
} // namespace nc
