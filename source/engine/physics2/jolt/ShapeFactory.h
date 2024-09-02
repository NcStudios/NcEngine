#pragma once

#include "Conversion.h"

#include "Jolt/Physics/Collision/Shape/BoxShape.h"
#include "Jolt/Physics/Collision/Shape/ScaledShape.h"
#include "Jolt/Physics/Collision/Shape/SphereShape.h"

namespace nc::physics
{
inline auto NormalizeScaleForShape(nc::physics::Shape shape, JPH::Vec3& scaleOut) -> bool
{
    switch (shape)
    {
        case nc::physics::Shape::Box:
        {
            return false;
        }
        case nc::physics::Shape::Sphere:
        {
            if (JPH::ScaleHelpers::IsUniformScale(scaleOut))
            {
                return false;
            }

            scaleOut = JPH::ScaleHelpers::MakeUniformScale(scaleOut);
            return true;
        }
        default:
        {
            NC_ASSERT(false, fmt::format("Unhandled Shape: '{}'", std::to_underlying(shape)));
            std::unreachable();
        }
    }
}

class ShapeFactory
{
    static constexpr auto boxConvexRadius = 0.05f;

    public:
        auto MakeShape(Shape shape, const JPH::Vec3& scale) -> JPH::Ref<JPH::Shape>
        {
            /** @todo: 692, 693, 694 support additional shape types */
            switch (shape)
            {
                case Shape::Box:
                    return MakeBox(scale * 0.5f);
                case Shape::Sphere:
                    NC_ASSERT(JPH::ScaleHelpers::IsUniformScale(scale), "Sphere requires uniform scale");
                    return MakeSphere(scale.GetX() * 0.5f);
                default:
                    NC_ASSERT(false, fmt::format("Unhandled Shape '{}'", std::to_underlying(shape)));
                    std::unreachable();
            };
        }

        auto MakeBox(const JPH::Vec3& halfExtents) -> JPH::Ref<JPH::Shape>
        {
            return JPH::Ref<JPH::Shape>{new JPH::BoxShape(halfExtents, boxConvexRadius)};
        }

        auto MakeSphere(float radius) -> JPH::Ref<JPH::Shape>
        {
            return JPH::Ref<JPH::Shape>{new JPH::SphereShape(radius)};
        }
};
} // namespace nc::physics
