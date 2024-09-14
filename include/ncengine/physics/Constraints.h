/**
 * @file Constraints.h
 * @copyright Jaremie Romer and McCallister Romer 2024
 */
#pragma once

#include "ncengine/ecs/Entity.h"

#include <variant>

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

/** @brief The space a Constraint is applied in. */
enum class ConstraintSpace : uint8_t
{
    World,
    Local
};

/** @brief Initialization information for a constraint that attaches two bodies with no degrees of freedom. */
struct FixedConstraintInfo
{
    Vector3 point1 = Vector3::Zero();
    Vector3 axisX1 = Vector3::Right();
    Vector3 axisY1 = Vector3::Up();
    Vector3 point2 = Vector3::Zero();
    Vector3 axisX2 = Vector3::Right();
    Vector3 axisY2 = Vector3::Up();
    bool autoDetect = false;
    ConstraintSpace space = ConstraintSpace::World;
};

/** @brief Initialization information for a constraint that attaches two bodies at a point. */
struct PointConstraintInfo
{
    Vector3 point1 = Vector3::Zero();
    Vector3 point2 = Vector3::Zero();
    ConstraintSpace space = ConstraintSpace::World;
};

/** @brief Generalized constraint initialization information. */
using ConstraintInfo = std::variant<FixedConstraintInfo, PointConstraintInfo>;

/** @brief Unique value identifying internal Constraint state. */
using ConstraintId = uint32_t;

/** @brief Information regarding an existing Constraint. */
struct ConstraintView
{
    ConstraintInfo info;
    Entity referencedEntity = Entity::Null();
    ConstraintId id = UINT32_MAX;
};
} // namespace nc::physics
