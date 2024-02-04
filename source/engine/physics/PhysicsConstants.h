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
constexpr float WarmstartFactor = 0.75f;          // Percentage of previous impulse used to initialize constraints

/** Contact Behavior */
constexpr float ContactBreakDistanceAlongNormal = -0.005f;        // How aggressively contact points are removed/retained
constexpr float ContactBreakDistanceAlongTangent = 0.05f;
constexpr float SquareContactBreakDistanceAlongTangent = ContactBreakDistanceAlongTangent * ContactBreakDistanceAlongTangent;
constexpr float PenetrationSlop = 0.005f;              // How far objects are allowed to penetrate
constexpr bool EnableBaumgarteStabilization = true;   // Enable velocity-based position correction
constexpr bool EnableDirectPositionCorrection = false; // Enable translation-based position correction
constexpr float BaumgarteFactor = 0.15f;               // Bias factor for baumgarte stabilization [0, 1]
constexpr float PositionCorrectionFactor = 0.05f;    // Percentage of mtv to use for direct position correction
constexpr bool EnableRestitutionSlop = true;          // Enabled zeroing of restitution below a threshold
constexpr float RestitutionSlop = 0.1f;               // Velocity threshold for applying restitution

/** Sleeping Behavior */
constexpr bool EnableSleeping = false;   // Allow disabling simulation of inactive bodies
constexpr float SleepEpsilon = 0.01f;    // Velocity threshold for physics sleeping
constexpr uint8_t FramesUntilSleep = 5u; // How many frames below epsilon until an object sleeps

#define NC_PHYSICS_DEBUGGING 0

#define NC_PHYSICS_DEBUG_CONTACT_POINTS 1 // Debug logging for contact points
} // namespace nc::physics
