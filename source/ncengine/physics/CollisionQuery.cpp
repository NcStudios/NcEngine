#include "CollisionQueryImpl.h"
#include "jolt/CollisionQueryContext.h"
#include "jolt/Conversion.h"
#include "jolt/CookedShapeUtility.h"
#include "jolt/ShapeFactory.h"

#include "Jolt/Physics/Collision/RayCast.h"
#include "Jolt/Physics/Collision/CastResult.h"

namespace nc
{
CollisionQueryImpl::CollisionQueryImpl(const CollisionQueryFilter& filter)
    : m_filter{filter, s_ctx->lock}
{
}

auto CollisionQueryImpl::CastRay(const Ray& ray) const -> RayCastResult
{
    const auto raycast = physics::ToRay(ray);
    auto result = JPH::RayCastResult{};
    if (s_ctx->query.CastRay(raycast, result, m_filter, m_filter, m_filter))
    {
        const auto point = raycast.GetPointOnRay(result.mFraction);
        const auto& body = JPH::BodyLockRead{s_ctx->lock, result.mBodyID}.GetBody();
        const auto normal = body.GetWorldSpaceSurfaceNormal(result.mSubShapeID2, point);
        return RayCastResult{
            Entity::FromHash(body.GetUserData()),
            physics::ToVector3(point),
            physics::ToVector3(normal)
        };
    }

    return RayCastResult{};
}

auto CollisionQueryImpl::TestShape(const Shape& shape,
                                   const Vector3& position,
                                   const Quaternion& rotation) -> TestShapeResult
{
    return TestShape(CookedShape{shape}, position, rotation);
}

auto CollisionQueryImpl::TestShape(const CookedShape& shape,
                                   const Vector3& position,
                                   const Quaternion& rotation) -> TestShapeResult
{
    const auto& internalShape = ShapeStorageRTTI::ToShape(shape.GetShapeData());
    auto collector = physics::ShapeCollector{s_ctx->lock};
    s_ctx->query.CollideShape(
        internalShape.GetPtr(),
        JPH::Vec3::sReplicate(1.0f),
        physics::ToJoltMatrix(rotation, position),
        JPH::CollideShapeSettings{},
        JPH::Vec3::sZero(),
        collector,
        m_filter,
        m_filter,
        m_filter
    );

    return TestShapeResult{collector.ExtractHits()};
}

auto CollisionQueryImpl::TestShape(const CookedShape& shape) -> TestShapeResult
{
    const auto& internalShape = ShapeStorageRTTI::ToShape(shape.GetShapeData());
    auto collector = physics::ShapeCollector{s_ctx->lock};
    s_ctx->query.CollideShape(
        internalShape.GetPtr(),
        JPH::Vec3::sReplicate(1.0f),
        physics::ToJoltMatrix(shape.GetRotation(), shape.GetPosition()),
        JPH::CollideShapeSettings{},
        JPH::Vec3::sZero(),
        collector,
        m_filter,
        m_filter,
        m_filter
    );

    return TestShapeResult{collector.ExtractHits()};
}

auto CollisionQueryImpl::TestPoint(const nc::Vector3& point) -> std::vector<nc::Entity>
{
    auto collector = physics::PointCollector{s_ctx->lock};
    s_ctx->query.CollidePoint(
        physics::ToJoltVec3(point),
        collector,
        m_filter,
        m_filter,
        m_filter
    );

    return collector.ExtractHits();
}

CollisionQuery::CollisionQuery(const CollisionQueryFilter& filter)
    : m_impl{std::make_unique<CollisionQueryImpl>(filter)}
{
}

CollisionQuery::CollisionQuery(CollisionQuery&&) noexcept = default;
CollisionQuery& CollisionQuery::operator=(CollisionQuery&&) noexcept = default;
CollisionQuery::~CollisionQuery() noexcept = default;

auto CollisionQuery::CastRay(const Ray& ray) const -> RayCastResult
{
    return m_impl->CastRay(ray);
}

auto CollisionQuery::TestShape(const Shape& shape,
                               const Vector3& position,
                               const Quaternion& rotation) const -> TestShapeResult
{
    return m_impl->TestShape(shape, position, rotation);
}

auto CollisionQuery::TestShape(const CookedShape& shape,
                               const Vector3& position,
                               const Quaternion& rotation) const -> TestShapeResult
{
    return m_impl->TestShape(shape, position, rotation);
}

auto CollisionQuery::TestShape(const CookedShape& shape) const -> TestShapeResult
{
    return m_impl->TestShape(shape);
}

auto CollisionQuery::TestPoint(const Vector3& point) const -> std::vector<Entity>
{
    return m_impl->TestPoint(point);
}
} // namespace nc
