#pragma once

#include "math/Vector.h"

namespace nc::physics
{
    constexpr size_t SolverIterations = 5u;        // Number of times to run physics solver
    constexpr float ContactBreakDistance = 0.001f; // How aggressively contact points are removed/retained
    constexpr float PenetrationSlop = 0.01f;       // How far objects are allowed to penetrate
    constexpr float WarmstartFactor = 0.5f;        // Percentage of previous impulse used to initialize constraints
    constexpr float SleepEpsilon = 0.01f;          // Velocity threshold for physics sleeping
    constexpr uint8_t FramesUntilSleep = 3u;       // How many frames below epsilon until an object sleeps
    constexpr float Gravity = -9.81f;              // Gravitational acceleration

    #define NC_PHYSICS_WARMSTARTING 1
}