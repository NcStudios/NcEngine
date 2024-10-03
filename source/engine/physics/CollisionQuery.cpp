#include "CollisionQueryImpl.h"
#include "jolt/CollisionQueryContext.h"
#include "jolt/Conversion.h"
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

auto CollisionQueryImpl::TestShape(const nc::Shape& shape) -> TestShapeResult
{
    auto internalShape = s_ctx->shapeFactory.MakeShape(shape, JPH::Vec3::sReplicate(1.0f));
    auto collector = physics::ShapeCollector{s_ctx->lock};
    s_ctx->query.CollideShape(
        internalShape,
        JPH::Vec3::sReplicate(1.0f),
        JPH::RMat44::sTranslation(internalShape->GetCenterOfMass()),
        JPH::CollideShapeSettings{},
        JPH::Vec3::sZero(),
        collector,
        m_filter,
        m_filter,
        m_filter
    );

    return TestShapeResult{collector.ExtractHits()};
}

CollisionQuery::CollisionQuery(const CollisionQueryFilter& filter)
    : m_impl{std::make_unique<CollisionQueryImpl>(filter)}
{
}

CollisionQuery::~CollisionQuery() noexcept = default;

auto CollisionQuery::CastRay(const Ray& ray) const -> RayCastResult
{
    return m_impl->CastRay(ray);
}

auto CollisionQuery::TestShape(const Shape& shape) const -> TestShapeResult
{
    return m_impl->TestShape(shape);
}
} // namespace nc
