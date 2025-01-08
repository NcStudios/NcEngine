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
#include "Jolt/Physics/Vehicle/VehicleConstraint.h"
#include "Jolt/Physics/Vehicle/WheeledVehicleController.h"

#include <ranges>

namespace
{
using namespace nc::physics;

auto MakeConstraint(const nc::FixedConstraintInfo& info,
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

auto MakeConstraint(const nc::PointConstraintInfo& info,
                    JPH::Body& first,
                    JPH::Body& second) -> JPH::Constraint*
{
    auto settings = JPH::PointConstraintSettings{};
    settings.mSpace = JPH::EConstraintSpace::LocalToBodyCOM;
    settings.mPoint1 = ToJoltVec3(info.ownerPosition);
    settings.mPoint2 = ToJoltVec3(info.targetPosition);
    return settings.Create(first, second);
}

auto MakeConstraint(const nc::DistanceConstraintInfo& info,
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

auto MakeConstraint(const nc::HingeConstraintInfo& info,
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

auto MakeConstraint(const nc::SliderConstraintInfo& info,
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

auto MakeConstraint(const nc::SwingTwistConstraintInfo& info,
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

auto MakeWheelSettings(const nc::WheelMount& wheelMount,
                       const nc::WheelSpec& wheelSpec,
                       const nc::Suspension& suspensionInfo) -> JPH::Ref<JPH::WheelSettingsWV>
{
    /**
     * @todo 863 We want to parameterize friction curves somehow - not sure what's the right way yet.
     *   possible default longitudinal:
     *     curve.AddPoint(0.0f, 0.0f);
     *     curve.AddPoint(0.06f, 1.2f);
     *     curve.AddPoint(0.2f, 1.0f);
     *   lateral:
     *     curve.AddPoint(0.0f, 0.0f);
     *     curve.AddPoint(3.0f, 1.2f);
     *     curve.AddPoint(20.0f, 1.0f);
     */
    static auto longitudinalFriction = [](){
        auto curve = JPH::LinearCurve{};
        curve.AddPoint(0.0f, 1.0f);
        return curve;
    }();

    static auto lateralFriction = [](){
        auto curve = JPH::LinearCurve{};
        curve.AddPoint(0.0f, 1.0f);
        return curve;
    }();

    auto out = JPH::Ref{new JPH::WheelSettingsWV()};
    out->mPosition = ToJoltVec3(wheelMount.position);
    out->mWheelUp = ToJoltVec3(wheelMount.up);
    out->mWheelForward = ToJoltVec3(wheelMount.forward);
    out->mSteeringAxis = ToJoltVec3(wheelMount.steeringAxis);
    out->mRadius = wheelSpec.radius;
    out->mWidth = wheelSpec.width;
    out->mInertia = wheelSpec.inertia;
    out->mAngularDamping = wheelSpec.damping;
    out->mMaxSteerAngle = wheelSpec.maxSteerAngle;
    out->mMaxBrakeTorque = wheelSpec.maxBrakeTorque;
    out->mMaxHandBrakeTorque = wheelSpec.maxHandBrakeTorque;
    out->mSuspensionDirection = ToJoltVec3(wheelMount.suspensionAxis);
    out->mSuspensionMinLength = suspensionInfo.minLength;
    out->mSuspensionMaxLength = suspensionInfo.maxLength;
    out->mSuspensionSpring = ToSpringSettings(suspensionInfo.spring);
    out->mLongitudinalFriction = longitudinalFriction;
    out->mLateralFriction = lateralFriction;
    return out;
};

// CAREFUL! - this is an owning raw pointer due to the way jolt manages wheel lifetimes
auto MakeWheel(const nc::WheelMount& wheelMount,
               const nc::WheelSpec& wheelSpec,
               const nc::Suspension& suspensionInfo) -> JPH::WheelWV*
{
    const auto settings = MakeWheelSettings(wheelMount, wheelSpec, suspensionInfo);
    return new JPH::WheelWV{*settings};
}

auto MakeDifferential(const nc::Differential& info,
                      int leftIndex,
                      int rightIndex) -> JPH::VehicleDifferentialSettings
{
    const auto torqueSplit = [leftIndex, rightIndex]() {
        if (leftIndex == -1)  return 1.0f; // single wheel, full power on right
        if (rightIndex == -1) return 0.0f; // single wheel, full power on left
        else                  return 0.5f; // two wheel, evenly split power
    }();

    return JPH::VehicleDifferentialSettings{
        leftIndex,
        rightIndex,
        info.ratio,
        torqueSplit,
        info.limitedSlipRatio
    };
}

auto FindDifferential(JPH::WheeledVehicleController::Differentials& differentials,
                      int leftIndex,
                      int rightIndex)
{
    return std::ranges::find_if(
        differentials,
        [leftIndex, rightIndex](const auto& differential){
            return differential.mLeftWheel == leftIndex &&
                   differential.mRightWheel == rightIndex;
        }
    );
}

void BalanceDifferentials(JPH::WheeledVehicleController::Differentials& differentials)
{
    // torque ratios must sum to 1, we just evenly split across each differential
    const auto count = static_cast<float>(differentials.size());
    const auto distributedTorqueRatio = 1.0f / count;
    for (auto& differential : differentials)
    {
        differential.mEngineTorqueRatio = distributedTorqueRatio;
    }
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

auto ConstraintFactory::MakeVehicleConstraint(VehicleInfo& info,
                                              JPH::Body& owner) -> JPH::Ref<JPH::VehicleConstraint>
{
    auto controller = JPH::Ref{new JPH::WheeledVehicleControllerSettings{}};
    SetVehicleEngineSettings(info.engine, controller->mEngine);
    SetVehicleTransmissionSettings(info.transmission, controller->mTransmission);

    auto settings = JPH::VehicleConstraintSettings{};
    SetVehicleOrientation(info.orientation, settings);

    auto& differentials = controller->mDifferentials;
    auto& wheels = settings.mWheels;
    auto wheelIndex = 0;
    for (auto& [left, right, spec, suspension, differential] : info.wheelAssemblies)
    {
        NC_ASSERT(
            left.IsEnabled() || right.IsEnabled(),
            "WheelAssembly must have at least one wheel enabled"
        );

        using Base = JPH::Ref<JPH::WheelSettings>;
        if (left.IsEnabled())
        {
            left.id = wheelIndex++;
            wheels.push_back(static_cast<Base>(MakeWheelSettings(left, spec, suspension)));
        }

        if (right.IsEnabled())
        {
            right.id = wheelIndex++;
            wheels.push_back(static_cast<Base>(MakeWheelSettings(right, spec, suspension)));
        }

        if (differential.IsEnabled())
        {
            differentials.push_back(MakeDifferential(differential, left.id, right.id));
        }
    }

    BalanceDifferentials(differentials);
    settings.mController = std::move(controller);
    auto out = JPH::Ref{new JPH::VehicleConstraint{owner, settings}};
    out->SetVehicleCollisionTester(new JPH::VehicleCollisionTesterRay(ObjectLayer::Dynamic));
    return out;
}

void SetVehicleOrientation(const VehicleOrientation& in,
                           JPH::VehicleConstraintSettings& out)
{
    out.mUp = ToJoltVec3(in.up);
    out.mForward = ToJoltVec3(in.forward);
    out.mMaxPitchRollAngle = in.maxRollAngle;
}

void SetVehicleEngineSettings(const VehicleEngine& in,
                              JPH::VehicleEngineSettings& out)
{
    out.mMaxTorque = in.maxTorque;
    out.mMinRPM = in.minRPM;
    out.mMaxRPM = in.maxRPM;
    out.mInertia = in.inertia;
    out.mAngularDamping = in.damping;
}

void SetVehicleTransmissionSettings(const VehicleTransmission& in,
                                    JPH::VehicleTransmissionSettings& out)
{
    out.mSwitchTime = in.shiftTime;
    out.mSwitchLatency = in.shiftLatency;
    out.mShiftUpRPM = in.shiftUpRPM;
    out.mShiftDownRPM = in.shiftDownRPM;
    out.mClutchReleaseTime = in.clutchRelease;
    out.mClutchStrength = in.clutchStrength;

    out.mGearRatios.clear();
    out.mGearRatios.reserve(in.gears.size());
    for (const auto ratio : in.gears)
    {
        out.mGearRatios.push_back(ratio);
    }

    out.mReverseGearRatios.clear();
    out.mReverseGearRatios.reserve(in.reverseGears.size());
    for (const auto ratio : in.reverseGears)
    {
        out.mReverseGearRatios.push_back(ratio);
    }
}

void AddWheelAssembly(WheelAssembly& assembly,
                      JPH::VehicleConstraint& constraint,
                      JPH::WheeledVehicleController& controller)
{
    auto& [left, right, spec, suspension, differential] = assembly;
    NC_ASSERT(
        left.IsEnabled() || right.IsEnabled(),
        "WheelAssembly must have at least one enabled wheel."
    );

    auto& wheels = constraint.GetWheels();
    auto wheelIndex = static_cast<int>(wheels.size());

    if (left.IsEnabled())
    {
        left.id = wheelIndex++;
        wheels.push_back(MakeWheel(left, spec, suspension));
    }

    if (assembly.rightWheel.IsEnabled())
    {
        right.id = wheelIndex++;
        wheels.push_back(MakeWheel(right, spec, suspension));
    }

    if (differential.IsEnabled())
    {
        auto& differentials = controller.GetDifferentials();
        differentials.push_back(MakeDifferential(differential, left.id, right.id));
        BalanceDifferentials(differentials);
    }
}

void RemoveWheelAssembly(size_t assemblyIndex,
                         std::vector<WheelAssembly>& assemblies,
                         JPH::VehicleConstraint& constraint,
                         JPH::WheeledVehicleController& controller)
{
    auto& toRemove = assemblies.at(assemblyIndex);
    const auto leftIndex = toRemove.leftWheel.id;
    const auto rightIndex = toRemove.rightWheel.id;
    const auto largestIndex = std::max(leftIndex, rightIndex);
    const auto removeCount = static_cast<int>(toRemove.WheelCount());

    // remove wheels from constraint
    {
        auto& wheels = constraint.GetWheels();
        const auto removeBegin = toRemove.leftWheel.IsEnabled() ? leftIndex : rightIndex;
        const auto removeEnd = largestIndex + 1;
        wheels.erase(wheels.begin() + removeBegin, wheels.begin() + removeEnd);
    }

    // fix cached wheel ids
    for (auto& after : assemblies)
    {
        if (after.leftWheel.id  > largestIndex) after.leftWheel.id  -= removeCount;
        if (after.rightWheel.id > largestIndex) after.rightWheel.id -= removeCount;
    }

    // remove differential from controller
    auto& differentials = controller.GetDifferentials();
    if (toRemove.IsPowered())
    {
        differentials.erase(FindDifferential(differentials, leftIndex, rightIndex));
        BalanceDifferentials(differentials);
    }

    // fix differential wheel ids
    for (auto& differential : differentials)
    {
        if (differential.mLeftWheel  > largestIndex) differential.mLeftWheel  -= removeCount;
        if (differential.mRightWheel > largestIndex) differential.mRightWheel -= removeCount;
    }

    // erase from assembly settings
    assemblies.erase(assemblies.begin() + assemblyIndex);
}

void ModifyWheelAssembly(const WheelAssembly& assembly,
                         JPH::VehicleConstraint& constraint,
                         JPH::WheeledVehicleController& controller)
{
    const auto& [left, right, spec, suspension, differential] = assembly;
    auto& wheels = constraint.GetWheels();
    if (left.IsEnabled())
    {
        const auto newWheel = MakeWheel(left, spec, suspension);
        const auto oldWheel = std::exchange(wheels.at(left.id), newWheel);
        delete oldWheel; // these are owning ptrs but not ref counted
    }

    if (right.IsEnabled())
    {
        const auto newWheel = MakeWheel(right, spec, suspension);
        const auto oldWheel = std::exchange(wheels.at(right.id), newWheel);
        delete oldWheel;
    }

    // update differential state based on what changed in the settings:
    //   - disabled in constraint and settings -> no action
    //   - disabled in constraint but enabled in settings -> build new one
    //   - enabled in constraint and settings -> rebuild it
    //   - enabled in constraint but disabled in settings -> remove it
    auto& differentials = controller.GetDifferentials();
    auto differentialPos = FindDifferential(differentials, left.id, right.id);
    if (differentialPos == differentials.cend())
    {
        if (!differential.IsEnabled())
        {
            return;
        }

        differentials.push_back(MakeDifferential(differential, left.id, right.id));
    }
    else if (differential.IsEnabled())
    {
        *differentialPos = MakeDifferential(differential, left.id, right.id);
    }
    else
    {
        differentials.erase(differentialPos);
    }

    BalanceDifferentials(differentials);
}
} // namespace nc::physics
