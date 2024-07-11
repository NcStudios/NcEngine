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
constexpr size_t EpaMaxIterations = 32u;             // Number of iterations aftwer which to give up trying to find contact points
constexpr float EpaTolerance = 0.00001f;             // Error threshold for finding contact points (> 0)
constexpr float ContactBreakDistance = -0.01f;       // How aggressively contact points are removed/retained (< 0)
constexpr bool EnableStickyContacts = true;          // Minimize how many contacts points are discarded due to tangential separation (only for dynamic box/hull vs static box/hull)
constexpr float StickyContactBreakDistance = -0.5f;  // How aggressively sticky contact points are removed/retained (< 0)
constexpr float StickyContactMaxDistance = -1.0f;    // Always discard contacts sticky contacts beyond this threshold (< StickyContactBreakDistance)

constexpr float PenetrationSlop = 0.005f;               // How far objects are allowed to penetrate (>= 0)
constexpr bool EnableBaumgarteStabilization = true;     // Enable velocity-based position correction
constexpr float BaumgarteFactor = 0.3f;                 // Bias factor for baumgarte stabilization [0, 1]
constexpr bool EnableDirectPositionCorrection = false;  // Enable translation-based position correction
constexpr float PositionCorrectionFactor = 0.2f;        // Percentage of minimum translation vector to use for direct position correction [0, 1]
constexpr float MaxLinearPositionCorrection = 0.2f;     // Maximum translation distance for direct position correction (> 0)
constexpr bool EnableRestitutionSlop = true;            // Enable zeroing of restitution below a threshold
constexpr float RestitutionSlop = 0.01f;                // Velocity threshold for applying restitution (>= 0)

/** Sleeping Behavior */
constexpr bool EnableSleeping = false;   // Allow disabling simulation of inactive bodies
constexpr float SleepEpsilon = 0.01f;    // Velocity threshold for physics sleeping
constexpr uint8_t FramesUntilSleep = 5u; // How many frames below epsilon until an object sleeps
} // namespace nc::physics