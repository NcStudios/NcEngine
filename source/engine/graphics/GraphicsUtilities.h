#pragma once

#include "ncmath/Vector.h"

namespace nc::graphics
{
constexpr auto AspectRatio = 16.0f / 9.0f;
auto AdjustDimensionsToAspectRatio(const nc::Vector2& dimensions) -> nc::Vector2;
} // namespace nc::graphics
