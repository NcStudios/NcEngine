#pragma once

#include "Conversion.h"

#include "Jolt/Physics/Collision/Shape/BoxShape.h"
#include "Jolt/Physics/Collision/Shape/SphereShape.h"

namespace nc::physics
{
inline auto MakeShape(Shape shape, const JPH::Vec3& transformScale) -> JPH::Ref<JPH::Shape>
{
    switch (shape)
    {
        case Shape::Box:
        {
            constexpr auto convexRadius = 0.05f;
            auto settings = JPH::BoxShapeSettings{transformScale * 0.5f, convexRadius};
            auto result = settings.Create();
            return result.Get();
        }
        case Shape::Sphere:
        {
            auto settings = JPH::SphereShapeSettings(transformScale.GetX() * 0.5f);
            auto result = settings.Create();
            return result.Get();
        }
        /** @todo: 692, 693, 694 support additional shape types */
    };

    throw NcError(fmt::format("Unhandled Shape '{}'", std::to_underlying(shape)));
}
} // namespace nc::physics
