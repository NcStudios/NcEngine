#pragma once

#include "ncengine/physics/HitInfo.h"

#include "Jolt/Jolt.h"
#include "Jolt/Physics/Collision/ContactListener.h"

#include <mutex>
#include <span>
#include <unordered_map>
#include <vector>

namespace JPH
{
class PhysicsSystem;
} // namespace JPH

namespace nc::physics
{
struct OverlappingPair
{
    uint64_t hash;
    Entity first;
    Entity second;
};

struct CollisionPair
{
    OverlappingPair pair;
    HitInfo hit;
};

class ContactListener final : public JPH::ContactListener
{
    public:
        explicit ContactListener(JPH::PhysicsSystem& physicsSystem)
            : m_physicsSystem{&physicsSystem}
        {
        }

        void OnContactAdded(const JPH::Body& body1,
                            const JPH::Body& body2,
                            const JPH::ContactManifold& manifold,
                            JPH::ContactSettings& settings) override;

        void OnContactRemoved(const JPH::SubShapeIDPair& pair) override;

        auto GetAdded() const -> std::span<const CollisionPair> { return m_added; }
        auto GetRemoved() const -> std::span<const OverlappingPair> { return m_removed; }
        void CommitPendingChanges();

    private:
        JPH::PhysicsSystem* m_physicsSystem;
        std::unordered_map<size_t, OverlappingPair> m_pairs;

        std::vector<CollisionPair> m_added;
        std::mutex m_addedMutex;

        std::vector<OverlappingPair> m_removed;
        std::mutex m_removedMutex;
};
} // namespace nc::physics
