/**
 * @file PhysicsLimits.h
 * @copyright Jaremie Romer and McCallister Romer 2024
 */
#pragma once

#include <limits>
#include <numbers>

namespace nc
{
/** @name Shape Limits */
constexpr auto g_minShapeScale = 0.1f;                  ///< minimum allowed scale for a Shape
constexpr auto g_maxShapeScale = 2000.0f;               ///< maximum allowed scale for a Shape
constexpr auto g_preferredMaxDynamicShapeScale = 10.0f; ///< maximum scale for a Shape on a dynamic RigidBody (soft limit for best stability)

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
constexpr auto g_maxLinearVelocity = 500.0f;                             ///< maximum linear velocity a RigidBody can reach
constexpr auto g_maxAngularVelocity = std::numbers::pi_v<float> * 15.0f; ///< maximum angular velocity a RigidBody can reach

/** @name Constraint Limits */
constexpr auto g_minFrictionForce = 0.0f;                               ///< minimum friction force a constraint can apply
constexpr auto g_maxFrictionForce = std::numeric_limits<float>::max();  ///< maximum friction force a constraint can apply
constexpr auto g_minFrictionTorque = 0.0f;                              ///< minimum torque a constraint can apply as friction
constexpr auto g_maxFrictionTorque = std::numeric_limits<float>::max(); ///< maximum torque a constraint can apply as friction
} // namespace nc
