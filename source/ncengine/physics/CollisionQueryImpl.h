#pragma once

#include "jolt/CollisionQueryUtility.h"

#include "ncengine/physics/CollisionQuery.h"

namespace nc
{
namespace physics
{
struct CollisionQueryContext;
} // namespace physics

// CollisionQuery Pimpl
// note - Interface is exposed here just to keep the context pointer out of the engine API
//        while still allowing manager/tests to set it.
class CollisionQueryImpl
{
    public:
        explicit CollisionQueryImpl(const CollisionQueryFilter& filter);

        auto CastRay(const Ray& ray) const -> RayCastResult;
        auto TestShape(const nc::Shape& shape) -> TestShapeResult;
        auto TestPoint(const Vector3& point) -> std::vector<Entity>;

        static void SetContext(physics::CollisionQueryContext* ctx)
        {
            s_ctx = ctx;
        }

    private:
        inline static physics::CollisionQueryContext* s_ctx = nullptr;

        physics::QueryFilter m_filter;
};
} // namespace nc
