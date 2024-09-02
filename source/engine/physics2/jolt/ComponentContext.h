#pragma once

#include "ShapeFactory.h"

#include "Jolt/Physics/Body/BodyInterface.h"

namespace nc::physics
{
struct ComponentContext
{
    JPH::BodyInterface& interface;
    ShapeFactory& shapeFactory;
};
} // namespace nc::physics
