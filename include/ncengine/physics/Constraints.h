/**
 * @file Constraints.h
 * @copyright Jaremie Romer and McCallister Romer 2024
 */
#pragma once

#include "ncmath/Vector.h"

namespace nc::physics
{
/**
 * @brief Constrain an object's linear and angular velocities.
 * 
 * Degrees of freedom may be restricted by setting freedom values on a per-axis basis. Values should be in the range
 * [0, 1] with 0 fully disabling motion, 1 fully enabling motion, and intermediate values damping motion. The constraint
 * acts upon an object's velocities and does not restrict translations or rotations applied directly to a Transform. It
 * has no effect if the object does not have a PhysicsBody.
 */
struct VelocityRestriction
{
    Vector3 linearFreedom = Vector3::One();
    Vector3 angularFreedom = Vector3::One();
    bool worldSpace = true;
};

/**
 * @brief Constrain an object's position with a harmonic oscillator.
 * 
 * A PositionClamp keeps an object at a fixed worldspace position using a spring-damper system, allowing for some sag
 * and bounciness in the response. The damping ratio controls the oscillation decay (usually in the range [0, 1], or
 * greater for overdamping), and the damping frequency controls the rate of oscillation in hertz. The clamp acts on
 * velocities and has no effect if the object does not have a PhysicsBody.
 */
struct PositionClamp
{
    Vector3 targetPosition = Vector3::Zero();
    float dampingRatio = 1.0f;
    float dampingFrequency = 10.0f;
    float slackDistance = 0.0f;
    float slackDamping = 0.25f;
};

/**
 * @brief Constrain an object's orientation with a harmonic oscillator.
 * 
 * An OrientationClamp keeps an object's local up axis fixed to a target direction using a spring-damper system, allowing
 * for some sag and bounciness in the response. The damping ratio controls the oscillation decay (usually in the range
 * [0, 1], or greater for overdamping), and the damping frequency controls the rate of oscillation in hertz. The clamp
 * acts on velocities and has no effect if the object does not have a PhysicsBody.
 */
struct OrientationClamp
{
    Vector3 targetOrientation = Vector3::Up();
    float dampingRatio = 0.1f;
    float dampingFrequency = 10.0f;
};
} // namespace nc::physics
