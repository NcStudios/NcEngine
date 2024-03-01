#pragma once

#include <cstdint>

namespace nc::physics
{
/** General Behavior */
constexpr size_t MaxPhysicsIterations = 3u; // Maximum number of times physics can run per game loop iteration
constexpr float Gravity = -10.0f;           // Gravitational acceleration
constexpr float MaxAngularVelocity = 7.0f;  // Value angular velocities are clamped to (radians/second)

/** Solver Convergence */
constexpr size_t SolverIterations = 5u;          // Number of times to run physics solver on constraints
constexpr bool EnableContactWarmstarting = true; // Start contact resolution from the most recent impulse or 0
constexpr bool EnableJointWarmstarting  = true;  // Start joint resolution from the most recent impulse or 0
constexpr float WarmstartFactor = 0.75f;         // Percentage of previous impulse used to initialize constraints

/** Contact Behavior */
constexpr size_t EpaMaxIterations = 32u;                     // Number of iterations aftwerwhich to give up trying to find contact points
constexpr float EpaTolerance = 0.00001f;                     // Error threshold for finding contact points
constexpr float ContactBreakDistance = -0.01f;               // How aggressively contact points are removed/retained
constexpr bool PreferSingleTangentContactBreak = false;      // Try to only discard only one contact due to tangential separation (potentially more stable for sliding scenarios)
constexpr float MandatoryTangentContactBreakDistance = 0.1f; // Always discard contacts due to tangential separation beyond this threshold, regardless of PreferSingleTangentContactBreak
constexpr float PenetrationSlop = 0.005f;                    // How far objects are allowed to penetrate
constexpr bool EnableBaumgarteStabilization = true;          // Enable velocity-based position correction
constexpr float BaumgarteFactor = 0.2f;                      // Bias factor for baumgarte stabilization [0, 1]
constexpr bool EnableDirectPositionCorrection = true;        // Enable translation-based position correction
constexpr float PositionCorrectionFactor = 0.2f;             // Percentage of mtv to use for direct position correction
constexpr float MaxLinearPositionCorrection = 0.2f;          // Maximum translation distance for direct position correction
constexpr bool EnableRestitutionSlop = true;                 // Enable zeroing of restitution below a threshold
constexpr float RestitutionSlop = 0.001f;                    // Velocity threshold for applying restitution

/** Sleeping Behavior */
constexpr bool EnableSleeping = false;   // Allow disabling simulation of inactive bodies
constexpr float SleepEpsilon = 0.01f;    // Velocity threshold for physics sleeping
constexpr uint8_t FramesUntilSleep = 5u; // How many frames below epsilon until an object sleeps
} // namespace nc::physics
