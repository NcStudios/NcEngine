/**
 * @file CollisionQuery.h
 * @copyright Jaremie Romer and McCallister Romer 2024
 */
#pragma once

#include "ncengine/ecs/Entity.h"

#include <functional>
#include <memory>
#include <vector>

namespace nc
{
struct Shape;

/** @brief Ray used for performing raycast queries. */
struct Ray
{
    Vector3 origin;    ///< origin of the ray
    Vector3 direction; ///< direction and distance of the ray
};

/** @brief Parameter controlling inclusion and exclusion of bodies from a CollisionQuery. */
struct CollisionQueryFilter
{
    using EntityFilter_t = std::function<bool(Entity)>;

    EntityFilter_t entityFilter = nullptr; ///< custom function to exclude bodies based on the parent Entity
    bool includeStatic = true;             ///< include or exclude static bodies from the query
    bool includeDynamic = true;            ///< include or exclude dynamic bodies from the query
    bool includeTrigger = false;           ///< include or exclude triggers from the query
};

/** @brief Result of a ray cast query. */
struct RayCastResult
{
    Entity hitBody = Entity::Null();         ///< the first hit body that passes all filters or Entity::Null()
    Vector3 hitPoint = Vector3::Zero();      ///< worldspace position where the ray intersects the body
    Vector3 surfaceNormal = Vector3::Zero(); ///< surface normal on the hit body
};

/** @brief Individual hit information from a shape collide query. */
struct TestShapeHit
{
    Entity hitBody;          ///< the body that was collided with
    Vector3 hitPoint;        ///< contact point on the surface of the hit body
    Vector3 collisionNormal; ///< normal along which the collision would be resolved (towards hitBody)
    float depth;             ///< penetration deptch of the collision
};

/** @brief Result of a shape collide query. */
struct TestShapeResult
{
    std::vector<TestShapeHit> hits; ///< 0 or more hit results
};

/** @brief Interface for performing collision querries against \ref RigidBody "rigid bodies". */
class CollisionQuery
{
    public:
        /** @brief Construct a CollisionQuery capable of testing against bodies which pass the specified filter. */
        explicit CollisionQuery(const CollisionQueryFilter& filter = CollisionQueryFilter{});
        ~CollisionQuery() noexcept;

        /** @brief Test for intersection with a ray returning information on the first hit RigidBody. */
        auto CastRay(const Ray& ray) const -> RayCastResult;

        /** @brief Test for collisions against a shape returning information on all hit \ref RigidBody "rigid bodies". */
        auto TestShape(const Shape& shape) const -> TestShapeResult;

    private:
        std::unique_ptr<class CollisionQueryImpl> m_impl;
};
} // namespace nc
