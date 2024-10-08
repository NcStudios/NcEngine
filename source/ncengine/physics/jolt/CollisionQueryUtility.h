#pragma once

#include "Conversion.h"
#include "Layers.h"

#include "Jolt/Physics/Collision/Shape/Shape.h"
#include "Jolt/Physics/Collision/CollidePointResult.h"
#include "Jolt/Physics/Collision/CollideShape.h"

#include <vector>

namespace nc::physics
{
inline auto GetEntity(const JPH::Body& body) -> Entity
{
    return Entity::FromHash(body.GetUserData());
}

inline auto GetEntity(const JPH::BodyLockInterfaceNoLock& lock, const JPH::BodyID& id) -> Entity
{
    return GetEntity(JPH::BodyLockRead{lock, id}.GetBody());
}

// Combined broad phase, object layer, and body/entity filter for collision queries.
class QueryFilter final : public JPH::BroadPhaseLayerFilter,
                          public JPH::ObjectLayerFilter,
                          public JPH::BodyFilter
{
    public:
        explicit QueryFilter(const nc::CollisionQueryFilter& filter,
                             const JPH::BodyLockInterfaceNoLock& lock)
            : m_filter{filter.entityFilter},
              m_lock{&lock},
              m_included{filter.includeStatic, filter.includeDynamic, filter.includeTrigger}
        {
            NC_ASSERT(m_filter, "CollisionQuery::entityFilter must be non-null.");
        }

        auto ShouldCollide(JPH::BroadPhaseLayer layer) const -> bool override
        {
            return m_included[static_cast<JPH::BroadPhaseLayer::Type>(layer)];
        }

        auto ShouldCollide(JPH::ObjectLayer layer) const -> bool override
        {
            return m_included[layer];
        }

        auto ShouldCollide(const JPH::BodyID& id) const -> bool override
        {
            return m_filter(GetEntity(*m_lock, id));
        }

        auto ShouldCollideLocked(const JPH::Body& body) const -> bool override
        {
            return m_filter(GetEntity(body));
        }

    private:
        static_assert(
            nc::physics::BroadPhaseLayer::LayerCount == nc::physics::ObjectLayer::LayerCount,
            "Invalid design assumptions"
        );

        nc::CollisionQueryFilter::EntityFilter_t m_filter;
        const JPH::BodyLockInterfaceNoLock* m_lock;
        bool m_included[nc::physics::BroadPhaseLayer::LayerCount];
};

// Builds a list of hit results for shape-based collision queries.
class ShapeCollector final : public JPH::CollideShapeCollector
{
    public:
        explicit ShapeCollector(const JPH::BodyLockInterfaceNoLock& lock)
            : m_lock{&lock}
        {
        }

        void AddHit(const JPH::CollideShapeResult& in) override
        {
            m_hits.emplace_back(
                GetEntity(*m_lock, in.mBodyID2),
                nc::physics::ToVector3(in.mContactPointOn2),
                nc::physics::ToVector3(in.mPenetrationAxis.Normalized()),
                in.mPenetrationDepth
            );
        }

        auto ExtractHits() -> std::vector<nc::TestShapeHit>&&
        {
            return std::move(m_hits);
        }

    private:
        std::vector<nc::TestShapeHit> m_hits;
        const JPH::BodyLockInterfaceNoLock* m_lock;
};

// Builds a list of hit results for point-based collision queries.
class PointCollector final : public JPH::CollidePointCollector
{
    public:
        explicit PointCollector(const JPH::BodyLockInterfaceNoLock& lock)
            : m_lock{&lock}
        {
        }

        void AddHit(const JPH::CollidePointResult& in) override
        {
            m_hits.push_back(GetEntity(*m_lock, in.mBodyID));
        }

        auto ExtractHits() -> std::vector<nc::Entity>&&
        {
            return std::move(m_hits);
        }

    private:
        std::vector<nc::Entity> m_hits;
        const JPH::BodyLockInterfaceNoLock* m_lock;
};
} // namespace nc::physics
