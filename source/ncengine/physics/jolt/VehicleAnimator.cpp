#include "VehicleAnimator.h"
#include "Conversion.h"

#include "Jolt/Jolt.h"
#include "Jolt/Physics/Vehicle/VehicleConstraint.h"
#include "Jolt/Physics/Vehicle/WheeledVehicleController.h"

namespace
{
using namespace DirectX;

auto CalculateWheelRotation(const JPH::Wheel& wheel) -> XMVECTOR
{
    const auto* settings = wheel.GetSettings();
    const auto right = nc::physics::ToXMVector(settings->mWheelUp.Cross(settings->mWheelForward).Normalized());
    const auto steerAxis = nc::physics::ToXMVector(settings->mSteeringAxis);
    const auto spinRotation = XMQuaternionRotationAxis(right, wheel.GetRotationAngle());
    const auto steerRotation = XMQuaternionRotationAxis(steerAxis, wheel.GetSteerAngle());
    return XMQuaternionMultiply(spinRotation, steerRotation);
}

auto CalculateWheelPosition(const JPH::Wheel& wheel) -> XMVECTOR
{
    const auto* settings = wheel.GetSettings();
    const auto attachPoint = nc::physics::ToXMVector(settings->mPosition);
    const auto direction = nc::physics::ToXMVector(settings->mSuspensionDirection);
    const auto offset = XMVectorScale(direction, wheel.GetSuspensionLength());
    return XMVectorAdd(attachPoint, offset);
}

template<class Func>
void VisitWheels(std::span<const nc::WheelAssembly> assemblies,
                 const JPH::VehicleConstraint& constraint,
                 nc::ecs::ComponentPool<nc::Transform>& transforms,
                 Func&& animate)
{
    for (const auto& [left, right, _1, _2, _3] : assemblies)
    {
        if (left.IsEnabled() && left.target.Valid())
        {
            animate(transforms.Get(left.target), *constraint.GetWheel(left.id));
        }

        if (right.IsEnabled() && right.target.Valid())
        {
            animate(transforms.Get(right.target), *constraint.GetWheel(right.id));
        }
    }
}
} // anonymous namespace

namespace nc::physics
{
void AnimateVehicle(std::span<const WheelAssembly> assemblies,
                    const JPH::VehicleConstraint& constraint,
                    ecs::ComponentPool<Transform>& transforms)
{
    VisitWheels(
        assemblies,
        constraint,
        transforms,
        [](nc::Transform& transform, const JPH::Wheel& wheel) {
            transform.SetPositionAndRotationXM(
                CalculateWheelPosition(wheel),
                CalculateWheelRotation(wheel)
            );
        }
    );
}

void AnimateVehicle(std::span<const WheelAssembly> assemblies,
                    const JPH::VehicleConstraint& constraint,
                    ecs::ComponentPool<Transform>& transforms,
                    float lerpFactor)
{
    VisitWheels(
        assemblies,
        constraint,
        transforms,
        [lerpFactor](nc::Transform& transform, const JPH::Wheel& wheel) {
            const auto& currentPosition = transform.LocalPositionXM();
            const auto& currentRotation = transform.LocalRotationXM();
            transform.SetPositionAndRotationXM(
                XMVectorLerp(currentPosition, CalculateWheelPosition(wheel), lerpFactor),
                XMQuaternionSlerp(currentRotation, CalculateWheelRotation(wheel), lerpFactor)
            );
        }
    );
}
} // namespace nc::physics
