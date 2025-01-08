#include "VehicleAnimator.h"

#include "Conversion.h"

#include "Jolt/Jolt.h"
#include "Jolt/Physics/Vehicle/VehicleConstraint.h"
#include "Jolt/Physics/Vehicle/WheeledVehicleController.h"

namespace
{
using namespace DirectX;

auto GetWheelRotation(const JPH::Wheel& wheel) -> XMVECTOR
{
    const auto* settings = wheel.GetSettings();
    const auto up = nc::physics::ToXMVector(settings->mWheelUp);
    const auto forward = nc::physics::ToXMVector(settings->mWheelForward);
    const auto right = XMVector3Normalize(XMVector3Cross(up, forward));
    const auto spinRotation = XMQuaternionRotationAxis(right, wheel.GetRotationAngle());
    const auto steerRotation = XMQuaternionRotationAxis(up, wheel.GetSteerAngle());
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
        GetWheelRotation(wheel)
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
} // namespace nc::physics
