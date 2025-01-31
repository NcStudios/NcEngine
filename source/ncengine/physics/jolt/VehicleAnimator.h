#pragma once

#include "ncengine/ecs/Transform.h"
#include "ncengine/ecs/ComponentPool.h"
#include "ncengine/physics/Vehicle.h"

namespace JPH
{
class VehicleConstraint;
} // namespace JPH

namespace nc
{
class PhysicsAnimator;

namespace physics
{
void AnimateVehicle(std::span<const WheelAssembly> assemblies,
                    const JPH::VehicleConstraint& constraint,
                    ecs::ComponentPool<Transform>& transformPool);

void AnimateVehicle(std::span<const WheelAssembly> assemblies,
                    const JPH::VehicleConstraint& constraint,
                    ecs::ComponentPool<Transform>& transformPool,
                    float lerpFactor);

void AnimateVehicle(std::span<const WheelAssembly> assemblies,
                    const JPH::VehicleConstraint& constraint,
                    PhysicsAnimator& animator);
} // namespace physics
} // namespace nc
