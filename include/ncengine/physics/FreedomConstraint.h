#pragma once

#include "ncengine/ecs/Entity.h"
#include "ncengine/utility/MatrixUtilities.h"

namespace nc::physics
{
struct DampingRatio
{
    static constexpr auto Soft = 0.3f;
    static constexpr auto Medium = 0.5f;
    static constexpr auto Hard = 1.0f;
};

struct DampingFrequency
{
    static constexpr auto Slow = 5.0f;
    static constexpr auto Medium = 10.0f;
    static constexpr auto Fast = 30.0f;
};

/**
 * @brief Constraint an object's linear and angular velocities.
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

/** @brief Fix an object's position with a harmonic oscillator. */
struct PositionClamp
{
    Vector3 targetPosition = Vector3::Zero();
    float dampingRatio = 1.0f;
    float dampingFrequency = 10.0f;
};
} // namespace nc::physics
