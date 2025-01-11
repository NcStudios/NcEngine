#pragma once

#include "ConstraintManager.h"
#include "ShapeFactory.h"
#include "VehicleManager.h"

#include "Jolt/Jolt.h"
#include "Jolt/Physics/Body/BodyInterface.h"

namespace nc::physics
{
struct ComponentContext
{
    JPH::BodyInterface& interface;
    ShapeFactory& shapeFactory;
    ConstraintManager& constraintManager;
    VehicleManager& vehicleManager;
};
} // namespace nc::physics
