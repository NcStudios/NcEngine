#pragma once

#include <cstdint>

namespace nc::physics
{
    /** General Behavior */
    constexpr size_t MaxPhysicsIterations = 3u; // Maximum number of times physics can run per game loop iteration
    constexpr float Gravity = -9.81f;           // Gravitational acceleration
    constexpr float MaxAngularVelocity = 7.0f;  // Value angular velocities are clamped to (radians/second)

    /** Solver Convergence */
    constexpr size_t SolverIterations = 5u;          // Number of times to run physics solver on constraints
    constexpr bool EnableContactWarmstarting = true; // Start contact resolution from the most recent impulse or 0
    constexpr bool EnableJointWarmstarting  = true;  // Start joint resolution from the most recent impulse or 0
    constexpr float WarmstartFactor = 0.5f;          // Percentage of previous impulse used to initialize constraints

    /** Contact Tolerance */
    constexpr float ContactBreakDistance = -0.01f; // How aggressively contact points are removed/retained
    constexpr float PenetrationSlop = 0.01f;       // How far objects are allowed to penetrate

    /** Position Correction */
    constexpr bool EnableBaumgarteStabilization = true;   // Enable velocity-based position correction
    constexpr bool EnableDirectPositionCorrection = true; // Enable translation-based position correction
    constexpr float BaumgarteFactor = 0.1f;               // Bias factor for baumgarte stabilization [0, 1]
    constexpr float PositionCorrectionFactor = 0.333f;    // Percentage of mtv to use for direct position correction

    /** Sleeping Behavior */
    constexpr bool EnableSleeping = false;   // Allow disabling simulation of inactive bodies
    constexpr float SleepEpsilon = 0.01f;    // Velocity threshold for physics sleeping
    constexpr uint8_t FramesUntilSleep = 5u; // How many frames below epsilon until an object sleeps

    #define NC_PHYSICS_DEBUGGING 0
}