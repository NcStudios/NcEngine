/**
 * @file Constraints.h
 * @copyright Jaremie Romer and McCallister Romer 2024
 */
#pragma once

#include "ncengine/ecs/Entity.h"

#include <variant>

namespace nc::physics
{
class ConstraintManager;
class RigidBody;

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

/** @brief The relative space of a constraint. */
enum class ConstraintSpace : uint8_t
{
    World,
    Local
};

/** @brief Initialization information for a constraint that attaches two bodies with no degrees of freedom. */
struct FixedConstraintInfo
{
    Vector3 point1 = Vector3::Zero();               /// first body reference position
    Vector3 axisX1 = Vector3::Right();              /// first body reference right axis
    Vector3 axisY1 = Vector3::Up();                 /// first body reference up axis
    Vector3 point2 = Vector3::Zero();               /// second body reference frame
    Vector3 axisX2 = Vector3::Right();              /// second body reference right axis
    Vector3 axisY2 = Vector3::Up();                 /// second body reference up axis
    bool detectFromPositions = false;               /// auto calculate settings from body positions (requires ConstraintSpace::World)
    ConstraintSpace space = ConstraintSpace::World; /// space other settings are relative to
};

/** @brief Initialization information for a constraint that attaches two bodies at a point. */
struct PointConstraintInfo
{
    Vector3 point1 = Vector3::Zero();               /// first body constraint position
    Vector3 point2 = Vector3::Zero();               /// second body constraint position
    ConstraintSpace space = ConstraintSpace::World; /// space other settings are relative to
};

/** @brief Generalized constraint initialization information. */
using ConstraintInfo = std::variant<FixedConstraintInfo, PointConstraintInfo>;

/** @brief Unique value identifying internal Constraint state. */
using ConstraintId = uint32_t;

/**
 * @brief A physics constraint attaching the owning RigidBody to another. */
class Constraint
{
    public:
        explicit Constraint(const ConstraintInfo& info,
                            Entity otherBody,
                            ConstraintId id)
            : m_info{info},
              m_otherBody{otherBody},
              m_id{id}
        {
        }

        /**
         * @brief Get the ConstraintInfo.
         * @note Settings may be modified but changes won't take effect until NotifyUpdateInfo() is called.
         */
        auto GetInfo() -> ConstraintInfo& { return m_info; }
        auto GetInfo() const -> const ConstraintInfo& { return m_info; }

        /** @brief Update internal state based on the current ConstraintInfo values. */
        void NotifyUpdateInfo();

        /** @brief Update the body that the Constraint owner is attached to (use nullptr to attach to the world). */
        void SetConstraintTarget(RigidBody* otherBody = nullptr);
        auto GetConstraintTarget() const -> Entity { return m_otherBody; }

        /** @brief Toggle the Constraint on or off. */
        void Enable(bool enabled);
        auto IsEnabled() const -> bool { return m_enabled; }

        /** @brief Get the Constraint's unique identifier. */
        auto GetId() const -> ConstraintId { return m_id; }

    private:
        friend class ConstraintManager;
        inline static ConstraintManager* s_manager = nullptr;

        ConstraintInfo m_info;
        Entity m_otherBody;
        ConstraintId m_id;
        bool m_enabled = true;
};
} // namespace nc::physics
