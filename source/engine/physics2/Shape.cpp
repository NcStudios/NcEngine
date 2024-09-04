#include "ncengine/physics/Shape.h"

#include "ncutility/NcError.h"

#include <utility>

namespace
{
void EnsureGreaterThanZeroScale(nc::Vector3& out)
{
    if (out.x <= 0.0f)
        out.x = nc::physics::g_minimumShapeScale;
    if (out.y <= 0.0f)
        out.y = nc::physics::g_minimumShapeScale;
    if (out.z <= 0.0f)
        out.z = nc::physics::g_minimumShapeScale;
}

void FixSphereScale(const nc::Vector3& current, nc::Vector3& desired)
{
    if (!nc::FloatEqual(current.x, desired.x))
        desired =  nc::Vector3::Splat(desired.x);
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
    auto allowedScale = newScale;
    EnsureGreaterThanZeroScale(allowedScale);

    switch (shape)
    {
        case nc::physics::ShapeType::Box:
        {
            break;
        }
        case nc::physics::ShapeType::Sphere:
        {
            if (!HasUniformElements(allowedScale))
            {
                FixSphereScale(currentScale, allowedScale);
            }

            break;
        }
        case nc::physics::ShapeType::Capsule:
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
} // namespace nc::physics
