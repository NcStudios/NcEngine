#include "ConstraintFactory.h"
#include "Conversion.h"

#include "Jolt/Jolt.h"
#include "Jolt/Physics/Constraints/DistanceConstraint.h"
#include "Jolt/Physics/Constraints/FixedConstraint.h"
#include "Jolt/Physics/Constraints/HingeConstraint.h"
#include "Jolt/Physics/Constraints/PointConstraint.h"
#include "Jolt/Physics/Constraints/SliderConstraint.h"
#include "Jolt/Physics/Constraints/SwingTwistConstraint.h"
#include "Jolt/Physics/PhysicsSystem.h"

namespace
{
using namespace nc::physics;

auto MakeConstraint(const FixedConstraintInfo& info,
                    JPH::Body& first,
                    JPH::Body& second) -> JPH::Constraint*
{
    auto settings = JPH::FixedConstraintSettings{};
    settings.mSpace = JPH::EConstraintSpace::LocalToBodyCOM;
    settings.mPoint1 = ToJoltVec3(info.ownerPosition);
    settings.mAxisX1 = ToJoltVec3(info.ownerRight);
    settings.mAxisY1 = ToJoltVec3(info.ownerUp);
    settings.mPoint2 = ToJoltVec3(info.targetPosition);
    settings.mAxisX2 = ToJoltVec3(info.targetRight);
    settings.mAxisY2 = ToJoltVec3(info.targetUp);
    return settings.Create(first, second);
}

auto MakeConstraint(const PointConstraintInfo& info,
                    JPH::Body& first,
                    JPH::Body& second) -> JPH::Constraint*
{
    auto settings = JPH::PointConstraintSettings{};
    settings.mSpace = JPH::EConstraintSpace::LocalToBodyCOM;
    settings.mPoint1 = ToJoltVec3(info.ownerPosition);
    settings.mPoint2 = ToJoltVec3(info.targetPosition);
    return settings.Create(first, second);
}

auto MakeConstraint(const DistanceConstraintInfo& info,
                    JPH::Body& first,
                    JPH::Body& second) -> JPH::Constraint*
{
    NC_ASSERT(info.minLimit >= 0.0f && info.maxLimit >= info.minLimit, "Invalid DistanceConstraintInfo limits");
    auto settings = JPH::DistanceConstraintSettings{};
    settings.mSpace = JPH::EConstraintSpace::LocalToBodyCOM;
    settings.mPoint1 = ToJoltVec3(info.ownerPosition);
    settings.mPoint2 = ToJoltVec3(info.targetPosition);
    settings.mMinDistance = info.minLimit;
    settings.mMaxDistance = info.maxLimit;
    settings.mLimitsSpringSettings = ToSpringSettings(info.springSettings);
    return settings.Create(first, second);
}

auto MakeConstraint(const HingeConstraintInfo& info,
                    JPH::Body& first,
                    JPH::Body& second) -> JPH::Constraint*
{
    NC_ASSERT(
        info.minLimit <= 0.0f &&
        info.maxLimit >= 0.0f &&
        info.maxLimit > info.minLimit,
        "Invalid HingeConstraintInfo limits"
    );

    auto settings = JPH::HingeConstraintSettings{};
    settings.mSpace = JPH::EConstraintSpace::LocalToBodyCOM;
    settings.mPoint1 = ToJoltVec3(info.ownerPosition);
    settings.mHingeAxis1 = ToJoltVec3(info.ownerHingeAxis);
    settings.mNormalAxis1 = ToJoltVec3(info.ownerNormalAxis);
    settings.mPoint2 = ToJoltVec3(info.targetPosition);
    settings.mHingeAxis2 = ToJoltVec3(info.targetHingeAxis);
    settings.mNormalAxis2 = ToJoltVec3(info.targetNormalAxis);
    settings.mLimitsMin = info.minLimit;
    settings.mLimitsMax = info.maxLimit;
    settings.mMaxFrictionTorque = info.maxFrictionTorque;
    settings.mLimitsSpringSettings = ToSpringSettings(info.springSettings);
    return settings.Create(first, second);
}

auto MakeConstraint(const SliderConstraintInfo& info,
                    JPH::Body& first,
                    JPH::Body& second) -> JPH::Constraint*
{
    NC_ASSERT(
        info.minLimit <= 0 &&
        info.maxLimit >= 0.0f &&
        info.maxLimit > info.minLimit,
        "Invalid SliderConstraintInfo limits"
    );

    auto settings = JPH::SliderConstraintSettings{};
    settings.mSpace = JPH::EConstraintSpace::LocalToBodyCOM;
    settings.mPoint1 = ToJoltVec3(info.ownerPosition);
    settings.mPoint2 = ToJoltVec3(info.targetPosition);
    settings.mSliderAxis1 = ToJoltVec3(info.ownerSliderAxis);
    settings.mNormalAxis1 = ToJoltVec3(info.ownerNormalAxis);
    settings.mSliderAxis2 = ToJoltVec3(info.targetSliderAxis);
    settings.mNormalAxis2 = ToJoltVec3(info.targetNormalAxis);
    settings.mLimitsMin = info.minLimit;
    settings.mLimitsMax = info.maxLimit;
    settings.mLimitsSpringSettings = ToSpringSettings(info.springSettings);
    settings.mMaxFrictionForce = info.maxFrictionForce;
    return settings.Create(first, second);
}

auto MakeConstraint(const SwingTwistConstraintInfo& info,
                    JPH::Body& first,
                    JPH::Body& second) -> JPH::Constraint*
{
    NC_ASSERT(
        info.swingLimit >= 0 &&
        info.swingLimit <= std::numbers::pi_v<float> &&
        info.twistLimit >= 0 &&
        info.twistLimit <= std::numbers::pi_v<float>,
        "Invalid SwingTwistConstraintInfo limits"
    );

    auto settings = JPH::SwingTwistConstraintSettings{};
    settings.mSpace = JPH::EConstraintSpace::LocalToBodyCOM;
    settings.mPosition1 = ToJoltVec3(info.ownerPosition);
    settings.mTwistAxis1 = ToJoltVec3(info.ownerTwistAxis);
    settings.mPlaneAxis1 = settings.mTwistAxis1.GetNormalizedPerpendicular();
    settings.mPosition2 = ToJoltVec3(info.targetPosition);
    settings.mTwistAxis2 = ToJoltVec3(info.targetTwistAxis);
    settings.mPlaneAxis2 = settings.mTwistAxis2.GetNormalizedPerpendicular();
    settings.mSwingType = JPH::ESwingType::Cone;
    settings.mNormalHalfConeAngle = info.swingLimit * 0.5f;
    settings.mPlaneHalfConeAngle = settings.mNormalHalfConeAngle;
    settings.mTwistMinAngle = -info.twistLimit;
    settings.mTwistMaxAngle = info.twistLimit;
    settings.mMaxFrictionTorque = info.maxFrictionTorque;
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
