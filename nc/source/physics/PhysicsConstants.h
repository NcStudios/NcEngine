#pragma once

#include "math/Vector.h"

namespace nc::physics
{
    constexpr size_t SolverIterations = 10u;
    constexpr float ContactBreakDistance = 0.001f;
    constexpr float PenetrationSlop = 0.05f;
    constexpr auto GravityAcceleration = Vector3{0.0f, -9.81f, 0.0f};
}