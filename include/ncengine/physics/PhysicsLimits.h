/**
 * @file PhysicsLimits.h
 * @copyright Jaremie Romer and McCallister Romer 2024
 */
#pragma once

#include <limits>
#include <numbers>

namespace nc::physics
{
/** @name Shape Limits */
constexpr auto g_minShapeScale = 0.1f;                  ///< minimum allowed scale for shapes
constexpr auto g_maxShapeScale = 2000.0f;               ///< maximum allowed scale for shapes
constexpr auto g_preferredMaxDynamicShapeScale = 10.0f; ///< maximum scale for dynamic physics shapes. (soft limit for best stability)

/** @name RigidBody Property Limits */
constexpr auto g_minMass = 0.1f;                   ///< minimum mass for a RigidBody
constexpr auto g_maxMass = 100000.0f;              ///< maximum mass for a RigidBody
constexpr auto g_minFrictionCoefficient = 0.0f;    ///< minimum friction coefficient
constexpr auto g_maxFrictionCoefficient = 1.0f;    ///< maximum friction coefficient
constexpr auto g_minRestitutionCoefficient = 0.0f; ///< minimum restitution coefficient
constexpr auto g_maxRestitutionCoefficient = 1.0f; ///< maximum restitution coefficient
constexpr auto g_minGravityMultiplier = 0.0f;      ///< minimum per-body gravity multiplier
constexpr auto g_maxGravityMultiplier = 100.0f;    ///< maximum per-body gravity multiplier
constexpr auto g_minDamping = 0.0f;                ///< minimum linear and angular damping value
constexpr auto g_maxDamping = 1.0f;                ///< maximum linear and angular damping value

/** @name Kinematic Limits */
constexpr auto g_maxLinearVelocity = 500.0f;                             ///< max linear velocity a RigidBody can reach
constexpr auto g_maxAngularVelocity = std::numbers::pi_v<float> * 15.0f; ///< max angular velocity a RigidBody can reach

/** @name Constraint Limits */
constexpr auto g_minFrictionForce = 0.0f;
constexpr auto g_maxFrictionForce = std::numeric_limits<float>::max();
constexpr auto g_minFrictionTorque = 0.0f;
constexpr auto g_maxFrictionTorque = std::numeric_limits<float>::max();
} // namespace nc::physics
