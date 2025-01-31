#include "VehicleAnimator.h"
#include "Conversion.h"

#include "ncengine/physics/PhysicsAnimator.h"

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

void AnimateWheel(nc::Transform& transform,
                  const JPH::Wheel& wheel)
{
    transform.SetPositionAndRotationXM(
        CalculateWheelPosition(wheel),
        CalculateWheelRotation(wheel)
    );
}

void AnimateWheel(nc::Transform& transform,
                  const JPH::Wheel& wheel,
                  float lerpFactor)
{
    const auto& currentPosition = transform.LocalPositionXM();
    const auto& currentRotation = transform.LocalRotationXM();
    transform.SetPositionAndRotationXM(
        XMVectorLerp(currentPosition, CalculateWheelPosition(wheel), lerpFactor),
        XMQuaternionSlerp(currentRotation, CalculateWheelRotation(wheel), lerpFactor)
    );
}
} // anonymous namespace

namespace nc::physics
{
void AnimateVehicle(std::span<const WheelAssembly> assemblies,
                    const JPH::VehicleConstraint& constraint,
                    ecs::ComponentPool<Transform>& transforms)
{
    for (const auto& [left, right, _1, _2, _3] : assemblies)
    {
        if (left.IsEnabled() && left.target.Valid())
        {
            AnimateWheel(transforms.Get(left.target), *constraint.GetWheel(left.id));
        }

        if (right.IsEnabled() && right.target.Valid())
        {
            AnimateWheel(transforms.Get(right.target), *constraint.GetWheel(right.id));
        }
    }
}

void AnimateVehicle(std::span<const WheelAssembly> assemblies,
                    const JPH::VehicleConstraint& constraint,
                    ecs::ComponentPool<Transform>& transforms,
                    float lerpFactor)
{
    for (const auto& [left, right, _1, _2, _3] : assemblies)
    {
        if (left.IsEnabled() && left.target.Valid())
        {
            AnimateWheel(transforms.Get(left.target), *constraint.GetWheel(left.id), lerpFactor);
        }

        if (right.IsEnabled() && right.target.Valid())
        {
            AnimateWheel(transforms.Get(right.target), *constraint.GetWheel(right.id), lerpFactor);
        }
    }
}

void AnimateVehicle(std::span<const WheelAssembly> assemblies,
                    const JPH::VehicleConstraint& constraint,
                    PhysicsAnimator& animator)
{
    for (const auto& [left, right, _1, _2, _3] : assemblies)
    {
        if (left.IsEnabled() && left.target.Valid())
        {
            const auto& wheel = *constraint.GetWheel(left.id);
            animator.Animate(left.target, CalculateWheelPosition(wheel), CalculateWheelRotation(wheel));
        }

        if (right.IsEnabled() && right.target.Valid())
        {
            const auto& wheel = *constraint.GetWheel(right.id);
            animator.Animate(right.target, CalculateWheelPosition(wheel), CalculateWheelRotation(wheel));
        }
    }
}
} // namespace nc::physics
