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
    const auto up = nc::physics::ToXMVector(settings->mWheelUp);
    const auto forward = nc::physics::ToXMVector(settings->mWheelForward);
    const auto right = XMVector3Normalize(XMVector3Cross(up, forward));
    const auto baseRotation = XMQuaternionRotationMatrix(XMMATRIX{right, up, forward, g_XMIdentityR3});
    const auto steerAxis = nc::physics::ToXMVector(settings->mSteeringAxis);
    const auto spinRotation = XMQuaternionRotationNormal(right, wheel.GetRotationAngle());
    const auto steerRotation = XMQuaternionRotationNormal(steerAxis, wheel.GetSteerAngle());
    const auto animatedRotation = XMQuaternionMultiply(spinRotation, steerRotation);
    return XMQuaternionMultiply(baseRotation, animatedRotation);
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
    constexpr auto wheelVisitor = [](auto& transform, const auto& wheel) {
        transform.SetPositionAndRotationXM(
            CalculateWheelPosition(wheel),
            CalculateWheelRotation(wheel)
        );
    };

    VisitWheels(assemblies, constraint, transforms, wheelVisitor);
}

void AnimateVehicle(std::span<const WheelAssembly> assemblies,
                    const JPH::VehicleConstraint& constraint,
                    ecs::ComponentPool<Transform>& transforms,
                    float lerpFactor)
{
    const auto wheelVisitor = [lerpFactor](auto& transform, const auto& wheel) {
        transform.SetPositionAndRotationXM(
            XMVectorLerp(transform.LocalPositionXM(), CalculateWheelPosition(wheel), lerpFactor),
            XMQuaternionSlerp(transform.LocalRotationXM(), CalculateWheelRotation(wheel), lerpFactor)
        );
    };

    VisitWheels(assemblies, constraint, transforms, wheelVisitor);
}
} // namespace nc::physics
