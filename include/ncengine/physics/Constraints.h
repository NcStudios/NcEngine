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
 * @brief Type of a Constraint.
 * @internal Keep ordered with ConstraintInfo types.
*/
enum class ConstraintType : uint8_t
{
    FixedConstraint,
    PointConstraint,
    DistanceConstraint,
    HingeConstraint,
    SliderConstraint,
    SwingTwistConstraint
};

/** @brief Settings for softening constraints with a spring-damper. */
struct SpringSettings
{
    /** @brief Spring frequency value hints */
    struct Frequency
    {
        static constexpr auto Disabled = 0.0f; ///< disable the spring
        static constexpr auto Soft = 2.0f;     ///< typical soft spring
        static constexpr auto Stiff = 20.0f;   ///< typical stiff spring
        static constexpr auto Max = 30.0f;     ///< frequency should not exceed half simulation frequency (assumes 60 fps here)
    };

    /** @brief Spring damping value hints */
    struct Damping
    {
        static constexpr auto Undamped = 0.0f;    ///< minimal energy loss, maximum oscillation (not completely lossless for stability)
        static constexpr auto Underdamped = 0.5f; ///< some energy loss, moderate oscillation (underdamping applies when 0 < d < 1)
        static constexpr auto Critical = 1.0f;    ///< full energy loss, no oscillation
        static constexpr auto Overdamped = 2.0f;  ///< energy loss occurs even outside of oscillation (overdamping applies when d > 1)
    };

    float frequency = Frequency::Disabled; ///< oscillation rate in hertz [0, targetFPS / 2]
    float damping = Damping::Undamped;     ///< oscillation decay [0, 1] (typical max is 1, but larger values are allowed)
};

/** @brief Constraint settings to rigidly fix two bodies together, limiting all relative motion. */
struct FixedConstraintInfo
{
    static constexpr auto type = ConstraintType::FixedConstraint;

    Vector3 ownerPosition = Vector3::Zero();  ///< local attach position
    Vector3 ownerRight = Vector3::Right();    ///< local right axis
    Vector3 ownerUp = Vector3::Up();          ///< local up axis
    Vector3 targetPosition = Vector3::Zero(); ///< local attach position
    Vector3 targetRight = Vector3::Right();   ///< local right axis
    Vector3 targetUp = Vector3::Up();         ///< local up axis
};

/** @brief Constraint settings to attach two bodies at a point, limiting relative motion to rotation only. */
struct PointConstraintInfo
{
    static constexpr auto type = ConstraintType::PointConstraint;

    Vector3 ownerPosition = Vector3::Zero();  ///< local attach position
    Vector3 targetPosition = Vector3::Zero(); ///< local attach position
};

/** @brief Constraint settings to keep two bodies within a specified distance range. */
struct DistanceConstraintInfo
{
    static constexpr auto type = ConstraintType::DistanceConstraint;

    Vector3 ownerPosition = Vector3::Zero();          ///< local attach position
    Vector3 targetPosition = Vector3::Zero();         ///< local attach position
    float minLimit = 0.0f;                            ///< how close together bodies are allowed to be [0, maxDistance]
    float maxLimit = 1.0f;                            ///< how far apart bodies are allowed to be [minDistance, infinity]
    SpringSettings springSettings = SpringSettings{}; ///< settings to soften the limits
};

/** @brief Constraint settings to attach two bodies with a hinge, limiting relative motion to rotation about a single axis. */
struct HingeConstraintInfo
{
    static constexpr auto type = ConstraintType::HingeConstraint;

    Vector3 ownerPosition = Vector3::Zero();          ///< local attach position
    Vector3 ownerHingeAxis = Vector3::Right();        ///< local axis of rotation
    Vector3 ownerNormalAxis = Vector3::Up();          ///< local reference axis perpendicular to ownerHingeAxis
    Vector3 targetPosition = Vector3::Zero();         ///< local attach position
    Vector3 targetHingeAxis = ownerHingeAxis;         ///< local axis of rotation
    Vector3 targetNormalAxis = ownerNormalAxis;       ///< local reference axis perpendicular to targetHingeAxis
    float minLimit = -std::numbers::pi_v<float>;      ///< min rotation about hinge axis [-pi, 0] ( |minLimit| + maxLimit > 0 )
    float maxLimit = std::numbers::pi_v<float>;       ///< max rotation about hinge axis [0, pi]
    float maxFrictionTorque = 0.0f;                   ///< max torque to apply as friction (Nm) [0, inf]
    SpringSettings springSettings = SpringSettings{}; ///< settings to soften the limits
};

/** @brief Constraint settings to attach two bodies with a slider, limiting relative motion to a single axis of translation. */
struct SliderConstraintInfo
{
    static constexpr auto type = ConstraintType::SliderConstraint;

    Vector3 ownerPosition = Vector3::Zero();          ///< local attach position
    Vector3 ownerSliderAxis = Vector3::Right();       ///< local axis of translation
    Vector3 ownerNormalAxis = Vector3::Up();          ///< local reference axis perpendicular to ownerSliderAxis
    Vector3 targetPosition = Vector3::Zero();         ///< local attach position
    Vector3 targetSliderAxis = ownerSliderAxis;       ///< local axis of translation
    Vector3 targetNormalAxis = ownerNormalAxis;       ///< local reference axis perpendicular to targetSliderAxis
    float minLimit = -1.0f;                           ///< slider length in negative direction [-inf, 0] ( |minLimit| + maxLimit > 0 )
    float maxLimit = 1.0f;                            ///< slider length in positive direction [0, inf]
    float maxFrictionForce = 0.0f;                    ///< max friction force that can be applied (N) [0, inf]
    SpringSettings springSettings = SpringSettings{}; ///< settings to soften the limits
};

/** @brief Constraint settings to attach two bodies with a shoulder-like joint, limiting relative motion to rotation within a cone. */
struct SwingTwistConstraintInfo
{
    static constexpr auto type = ConstraintType::SwingTwistConstraint;

    Vector3 ownerPosition = Vector3::Zero();      ///< local attach position
    Vector3 ownerTwistAxis = Vector3::Right();    ///< local twist axis (cone axis)
    Vector3 targetPosition = Vector3::Zero();     ///< local attach position
    Vector3 targetTwistAxis = Vector3::Right();   ///< local twist axis (cone axis)
    float swingLimit = std::numbers::pi_v<float>; ///< rotation limit from twist axis (cone angle) [0, pi]
    float twistLimit = std::numbers::pi_v<float>; ///< rotation limit about twist axis [0, pi]
    float maxFrictionTorque = 0.0f;               ///< max torque to apply as friction (Nm) [0, inf]
};

/**
 * @brief Generalized constraint initialization information.
 * @internal Keep ordered with ConstraintType.
 */
using ConstraintInfo = std::variant<FixedConstraintInfo,
                                    PointConstraintInfo,
                                    DistanceConstraintInfo,
                                    HingeConstraintInfo,
                                    SliderConstraintInfo,
                                    SwingTwistConstraintInfo>;

/** @brief Unique value identifying internal Constraint state. */
using ConstraintId = uint32_t;

/** @brief A physics constraint attaching the owning RigidBody to another. */
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

        /** @brief Get the type of constraint. */
        auto GetType() const -> ConstraintType;

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
