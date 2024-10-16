#pragma once

#include "ConstraintManager.h"
#include "ShapeFactory.h"

#include "Jolt/Jolt.h"
#include "Jolt/Physics/Body/BodyInterface.h"

namespace nc::physics
{
struct ComponentContext
{
    JPH::BodyInterface& interface;
    ShapeFactory& shapeFactory;
    ConstraintManager& constraintManager;
};
} // namespace nc::physics
