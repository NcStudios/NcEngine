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
    Vector3 origin = Vector3::Zero();     ///< origin of the ray
    Vector3 direction = Vector3::Front(); ///< combined direction and distance of the ray
};

/** @brief Parameter controlling inclusion and exclusion of bodies from a CollisionQuery. */
struct CollisionQueryFilter
{
    using EntityFilter_t = std::function<bool(Entity)>;
    static constexpr auto EntityFilterAll = [](Entity) { return true; };

    EntityFilter_t entityFilter = EntityFilterAll; ///< include only bodies passing a custom filter (non-null)
    bool includeStatic = true;                     ///< include static bodies in queries
    bool includeDynamic = true;                    ///< include dynamic and kinematic bodies in queries
    bool includeTrigger = false;                   ///< include triggers in queries
};

/** @brief Result of a raycast query. */
struct RayCastResult
{
    Entity hit = Entity::Null();             ///< parent Entity of the first hit body or Entity::Null()
    Vector3 point = Vector3::Zero();         ///< worldspace point where the ray intersects the body
    Vector3 surfaceNormal = Vector3::Zero(); ///< surface normal on the hit body
};

/** @brief Individual hit information from a shape test query. */
struct TestShapeHit
{
    Entity hit;              ///< the body that was collided with
    Vector3 point;           ///< worldspace contact point on the surface of the hit body
    Vector3 collisionNormal; ///< collision resolution normal pointing from shape towards hit
    float depth;             ///< penetration depth of the collision
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
