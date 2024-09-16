/**
 * @file PhysicsUtility.h
 * @copyright Jaremie Romer and McCallister Romer 2024
 */
#pragma once

#include "ncengine/physics/RigidBody.h"
#include "ncengine/physics/Shape.h"

#include <span>
#include <string_view>

namespace nc::physics
{
auto ToString(BodyType type) -> std::string_view;
auto ToBodyType(std::string_view bodyType) -> BodyType;
auto GetBodyTypeNames() -> std::span<const std::string_view>;

auto ToString(ShapeType type) -> std::string_view;
auto ToShapeType(std::string_view shapeType) -> ShapeType;
auto GetShapeTypeNames() -> std::span<const std::string_view>;

auto ToString(ConstraintType type) -> std::string_view;
auto ToConstraintType(std::string_view constraintType) -> ConstraintType;
auto GetConstraintTypeNames() -> std::span<const std::string_view>;

auto ToString(ConstraintSpace space) -> std::string_view;
auto ToConstraintSpace(std::string_view constraintSpace) -> ConstraintSpace;
auto GetConstraintSpaceNames() -> std::span<const std::string_view>;
} // namespace nc::physics
