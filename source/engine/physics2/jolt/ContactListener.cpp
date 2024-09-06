#include "ContactListener.h"
#include "Conversion.h"

#include "Jolt/Physics/PhysicsSystem.h"
#include "ncutility/NcError.h"

#include <ranges>

namespace
{
auto FillPoints(std::array<nc::Vector3, 4>& points, const JPH::ContactPoints& source) -> size_t
{
    const auto count = source.size();
    NC_ASSERT(count <= 4u, "Unexpected contact count");
    for (auto i = 0u; i < count; ++i)
    {
        points[i] = nc::physics::ToVector3(source[i]);
    }

    return count;
}
} // anonymous namespace

namespace nc::physics
{
// note: This is called from multiple threads with all bodies locked.
void ContactListener::OnContactAdded(const JPH::Body& body1,
                                     const JPH::Body& body2,
                                     const JPH::ContactManifold& manifold,
                                     JPH::ContactSettings&)
{
    if (m_physicsSystem->WereBodiesInContact(body1.GetID(), body2.GetID()))
    {
        return; // not the first contact between bodies
    }

    const auto hash = JPH::SubShapeIDPair{
        body1.GetID(),
        manifold.mSubShapeID1,
        body2.GetID(),
        manifold.mSubShapeID2
    }.GetHash();

    if (m_pairs.contains(hash))
    {
        return; // ignore wake up events
    }

    const auto pair = OverlappingPair{
        hash,
        Entity::FromHash(body1.GetUserData()),
        Entity::FromHash(body2.GetUserData())
    };

    const auto firstReceivesEvents = pair.first.ReceivesCollisionEvents() && !pair.first.IsStatic();
    const auto secondReceivesEvents = pair.second.ReceivesCollisionEvents() && !pair.second.IsStatic();
    if (!(firstReceivesEvents || secondReceivesEvents))
    {
        return; // our api doesn't allow this event to be dispatched
    }

    auto pointsOnFirst = std::array<Vector3, 4>{};
    auto pointsOnSecond = std::array<Vector3, 4>{};
    const auto numPointsOnFirst = FillPoints(pointsOnFirst, manifold.mRelativeContactPointsOn1);
    const auto numPointsOnSecond = FillPoints(pointsOnSecond, manifold.mRelativeContactPointsOn2);

    {
        auto lock = std::lock_guard{m_addedMutex};
        m_added.emplace_back(
            pair,
            HitInfo{
                ToVector3(manifold.mBaseOffset),
                ToVector3(manifold.mWorldSpaceNormal),
                manifold.mPenetrationDepth,
                Contacts{pointsOnFirst, numPointsOnFirst},
                Contacts{pointsOnSecond, numPointsOnSecond}
            }
        );
    }
}

// note: This called from multiple threads with all bodies locked.
void ContactListener::OnContactRemoved(const JPH::SubShapeIDPair& pairID)
{
    const auto id1 = pairID.GetBody1ID();
    const auto id2 = pairID.GetBody2ID();
    if (m_physicsSystem->WereBodiesInContact(id1, id2))
    {
        return; // not the last contact between bodies
    }

    auto& interface = m_physicsSystem->GetBodyInterfaceNoLock();
    if (interface.IsAdded(id1) && interface.IsAdded(id2))
    {
        if (!interface.IsActive(id1) && !interface.IsActive(id2))
        {
            return; // ignore sleep events
        }
    }

    const auto pos = m_pairs.find(pairID.GetHash());
    if (pos == m_pairs.cend())
    {
        return;
    }

    {
        auto lock = std::lock_guard{m_removedMutex};
        m_removed.push_back(pos->second);
    }
}

void ContactListener::CommitPendingChanges()
{
    for (const auto& overlappinPair : m_removed)
    {
        m_pairs.erase(overlappinPair.hash);
    }

    m_removed.clear();

    for (const auto& collisionPair : m_added)
    {
        m_pairs.emplace(collisionPair.pair.hash, collisionPair.pair);
    }

    m_added.clear();
}
} // namespace nc::physics
