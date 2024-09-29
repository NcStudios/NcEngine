#include "ncengine/physics/Shape.h"

#include "ncutility/NcError.h"

#include <utility>

namespace
{
auto ClampScale(const nc::Vector3& out) -> nc::Vector3
{
    return nc::Vector3{
        nc::Clamp(out.x, nc::g_minShapeScale, nc::g_maxShapeScale),
        nc::Clamp(out.y, nc::g_minShapeScale, nc::g_maxShapeScale),
        nc::Clamp(out.z, nc::g_minShapeScale, nc::g_maxShapeScale)
    };
}

void FixSphereScale(const nc::Vector3& current, nc::Vector3& desired)
{
    if (!nc::FloatEqual(current.x, desired.x))
        desired = nc::Vector3::Splat(desired.x);
    else if (!nc::FloatEqual(current.y, desired.y))
        desired = nc::Vector3::Splat(desired.y);
    else if (!nc::FloatEqual(current.z, desired.z))
        desired = nc::Vector3::Splat(desired.z);

    const auto avg = (desired.x + desired.y + desired.z) / 3.0f;
    desired = nc::Vector3::Splat(avg);
}

void FixCapsuleScale(const nc::Vector3& current, nc::Vector3& desired)
{
    if (!nc::FloatEqual(current.x, desired.x))
        desired.z = desired.x;
    else if (!nc::FloatEqual(current.z, desired.z))
        desired.x = desired.z;

    const auto avg = (desired.x + desired.z) / 2.0f;
    desired.x = avg;
    desired.z = avg;
}
} // anonymous namespace

namespace nc
{
auto NormalizeScaleForShape(ShapeType shape,
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
        default:
        {
            NC_ASSERT(false, fmt::format("Unhandled Shape: '{}'", std::to_underlying(shape)));
            std::unreachable();
        }
    }

    return allowedScale;
}
} // namespace nc
