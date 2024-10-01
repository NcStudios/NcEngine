#pragma once

#include "ncengine/physics/NcPhysics.h"

#include "Jolt/Jolt.h"
#include "Jolt/Physics/Body/BodyInterface.h"
#include "Jolt/Physics/Body/BodyLockInterface.h"

// #include "Jolt/Collision/CastResult.h"f
#include "Jolt/Physics/Collision/NarrowPhaseQuery.h"
#include "Jolt/Physics/Collision/RayCast.h"
#include "Jolt/Physics/Collision/CastResult.h"


#include "Conversion.h"

namespace nc::physics
{
// struct BroadPhaseLayerFilter : public JPH::BroadPhaseLayerFilter
// {

// };

// struct ObjectlayerFilter : public JPH::ObjectLayerFilter
// {

// }

class Caster
{
    public:
        auto CastRay(const JPH::NarrowPhaseQuery& query, const JPH::BodyInterface& interface, const Ray& ray) -> RayCastResult
        {
            auto result = JPH::RayCastResult{};
            const auto hit = query.CastRay(
                JPH::RRayCast{ToJoltVec3(ray.origin), ToJoltVec3(ray.direction)},
                result
            );

            if (hit)
            {
                auto hash = interface.GetUserData(result.mBodyID);
                return RayCastResult{Entity::FromHash(hash)};
            }

            return RayCastResult{};
        }

    private:

};
} // namespace nc::physics
