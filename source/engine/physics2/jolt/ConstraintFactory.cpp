#include "ConstraintFactory.h"
#include "Conversion.h"

#include "Jolt/Jolt.h"
#include "Jolt/Physics/Constraints/FixedConstraint.h"
#include "Jolt/Physics/Constraints/PointConstraint.h"
#include "Jolt/Physics/PhysicsSystem.h"

namespace
{
using namespace nc::physics;

auto MakeConstraint(const FixedConstraintInfo& info,
                    JPH::Body& first,
                    JPH::Body& second) -> JPH::Constraint*
{
    auto settings = JPH::FixedConstraintSettings{};
    settings.mSpace = ToConstraintSpace(info.space);
    if (info.autoDetect)
    {
        settings.mAutoDetectPoint = true;
    }
    else
    {
        settings.mPoint1 = ToJoltVec3(info.point1);
        settings.mAxisX1 = ToJoltVec3(info.axisX1);
        settings.mAxisY1 = ToJoltVec3(info.axisY1);
        settings.mPoint2 = ToJoltVec3(info.point2);
        settings.mAxisX2 = ToJoltVec3(info.axisX2);
        settings.mAxisY2 = ToJoltVec3(info.axisY2);
    }

    return settings.Create(first, second);
}

auto MakeConstraint(const PointConstraintInfo& info,
                    JPH::Body& first,
                    JPH::Body& second) -> JPH::Constraint*
{
    auto settings = JPH::PointConstraintSettings{};
    settings.mSpace = ToConstraintSpace(info.space);
    settings.mPoint1 = ToJoltVec3(info.point1);
    settings.mPoint2 = ToJoltVec3(info.point2);
    return settings.Create(first, second);
}
} // anonymous namespace

namespace nc::physics
{
auto ConstraintFactory::MakeConstraint(const ConstraintInfo& createInfo,
                                       JPH::Body& first,
                                       JPH::Body& second) -> JPH::Constraint*
{
    return std::visit(
        [&first, &second](auto&& unpacked)
        {
            return ::MakeConstraint(unpacked, first, second);
        },
        createInfo
    );
}
} // namespace nc::physics
