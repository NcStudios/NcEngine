/**
 * @file PhysicsUtility.h
 * @copyright Jaremie Romer and McCallister Romer 2024
 */
#pragma once

#include "RigidBody.h"
#include "Shape.h"

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
} // namespace nc::physics
