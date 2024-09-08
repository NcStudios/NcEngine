#include "ncengine/physics/Shape.h"

#include "ncutility/NcError.h"

#include <utility>

namespace
{
auto ClampScale(const nc::Vector3& out) -> nc::Vector3
{
    return nc::Vector3{
        nc::Clamp(out.x, nc::physics::g_minimumShapeScale, nc::physics::g_maximumShapeScale),
        nc::Clamp(out.y, nc::physics::g_minimumShapeScale, nc::physics::g_maximumShapeScale),
        nc::Clamp(out.z, nc::physics::g_minimumShapeScale, nc::physics::g_maximumShapeScale)
    };
}

void FixSphereScale(const nc::Vector3& current, nc::Vector3& desired)
{
    if (!nc::FloatEqual(current.x, desired.x))
        desired = nc::Vector3::Splat(desired.x);
    else if (!nc::FloatEqual(current.y, desired.y))
        desired = nc::Vector3::Splat(desired.y);
    else
        desired = nc::Vector3::Splat(desired.z);
}

void FixCapsuleScale(const nc::Vector3& current, nc::Vector3& desired)
{
    if (!nc::FloatEqual(current.x, desired.x))
        desired.z = desired.x;
    else
        desired.x = desired.z;
}
} // anonymous namespace

namespace nc::physics
{
auto NormalizeScaleForShape(nc::physics::ShapeType shape,
                            const Vector3& currentScale,
                            const Vector3& newScale) -> Vector3
{
    auto allowedScale = ClampScale(newScale);
    switch (shape)
    {
        case ShapeType::Box:
        {
            break;
        }
        case ShapeType::Sphere:
        {
            if (!HasUniformElements(allowedScale))
            {
                FixSphereScale(currentScale, allowedScale);
            }

            break;
        }
        case ShapeType::Capsule:
        {
            if (!FloatEqual(allowedScale.x, allowedScale.z))
            {
                FixCapsuleScale(currentScale, allowedScale);
            }

            break;
        }
        case ShapeType::ConvexHull:
        {
            // todo: not sure what's needed
            break;
        }
        default:
        {
            NC_ASSERT(false, fmt::format("Unhandled Shape: '{}'", std::to_underlying(shape)));
            std::unreachable();
        }
    }

    return allowedScale;
}
} // namespace nc::physics
