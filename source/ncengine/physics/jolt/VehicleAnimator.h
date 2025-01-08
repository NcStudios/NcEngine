#pragma once

#include "ncengine/ecs/Transform.h"
#include "ncengine/ecs/ComponentPool.h"
#include "ncengine/physics/Vehicle.h"

namespace JPH
{
class VehicleConstraint;
} // namespace JPH

namespace nc::physics
{
void AnimateVehicle(std::span<const WheelAssembly> assemblies,
                    const JPH::VehicleConstraint& constraint,
                    ecs::ComponentPool<Transform>& transformPool);
} // namespace nc::physics
