/**
 * @file Vehicle.h
 * @copyright Jaremie Romer and McCallister Romer 2025
 */
#pragma once

#include "ncengine/ecs/Entity.h"
#include "ncengine/physics/SpringSettings.h"

#include <initializer_list>
#include <span>
#include <vector>

namespace nc
{
/** @name Internal Vehicle handles. */
using VehicleId = uint32_t;
using VehicleHandle = void*;

/** @brief Settings for configuring a Vehicle orientation. */
struct VehicleOrientation
{
    Vector3 up = Vector3::Up();                     ///< local space vehicle up direction
    Vector3 forward = Vector3::Front();             ///< local space vehicle forward direction
    float maxRollAngle = std::numbers::pi_v<float>; ///< constrain vehicle pitch/roll [units: radians, range: [0, pi]] (pi to disable)
};

/** @brief Settings for a configuring a Vehicle engine. */
struct VehicleEngine
{
    float maxTorque = 500.0f; ///< engine torque limit [units: Nm, range: >= 0]
    float minRPM = 1000.0f;   ///< engine minimum rpm [range: [0, maxRPM]]
    float maxRPM = 6000.0f;   ///< engine maximum rpm [range: [minRPM, FLT_MAX]]
    float inertia = 0.5f;     ///< moment of inertia [units: kg m^2, range: > 0]
    float damping = 0.2f;     ///< angular damping factor [range: >= 0]
};

/** @brief Settings for a configuring a Vehicle transmission. */
struct VehicleTransmission
{
    std::vector<float> gears{defaultGears};  ///< forward gear ratios [range: > 0] (at least one gear required)
    std::vector<float> reverseGears{-2.90f}; ///< reverse gear ratios [range: < 0] (at least one gear required)
    float shiftTime = 0.5f;                  ///< time it takes to switch gears [units: s, range: >= 0]
    float shiftLatency = 0.5f;               ///< delay between gear shifts [units: s, range >= 0]
    float shiftUpRPM = 4000.0f;              ///< rpm at which upshift occurs [range: (shiftDownRPM, VehicleEninge::maxRPM]]
    float shiftDownRPM = 2000.0f;            ///< rpm at which downshift occurs [range: (0, shiftUpRPM)]
    float clutchRelease = 0.3f;              ///< time it takes to release clutch [units: s, range >= 0]
    float clutchStrength = 10.0f;            ///< strength of clutch when fully engaged (higher values handle more torque but are harder to engage) [units: k m^2 s^-1, range: >= 0]

    static constexpr auto defaultGears = std::initializer_list<float>{ 2.66f, 1.78f, 1.3f, 1.0f, 0.74f };
};

/** @brief Settings shared between all wheels in a WheelAssembly. */
struct WheelSpec
{
    float radius = 0.3f;                ///< wheel radius [units: m, range: > 0]
    float width = 0.1f;                 ///< wheel width [units: m, range: > 0)
    float inertia = 0.9f;               ///< moment of inertia [units: kg m^2, range: > 0] (for a cylinder: 0.5 * mass * radius^2)
    float damping = 0.2f;               ///< angular damping factor [range: >= 0]
    float maxSteerAngle = 0.523599f;    ///< how far wheel may turn [units: radians, range: [0, pi/2]]
    float maxBrakeTorque = 1500.0f;     ///< how much torque brake can apply [units: Nm, range: >=0];
    float maxHandBrakeTorque = 4000.0f; ///< how much torque hand brake can apply [units: Nm, range: >= 0];
};

/**
 * @brief Unique settings of a wheel in a WheelAssembly
 * 
 * Wheels may be disabled on initialization by setting id to -1. They cannot be later enabled/disabled. The id may be
 * read to check the state of the wheel, but should not otherwise be depended on (stability is not guaranteed).
 * 
 * An Entity may be automatically animated based on a wheel's rotation, steering angle and suspension length. When an
 * animation target is supplied, the following is assumed:
 * - It is a child of the owning RigidBody.
 * - It does not have its own RigidBody.
 * - It remains valid while assigned.
 * - Its Transform is not modified by user code while assigned.
 */
struct WheelMount
{
    int id = 0;                               ///< set to -1 to disable the wheel
    Vector3 position = Vector3::Zero();       ///< local space attach position relative to the owning RigidBody
    Vector3 up = Vector3::Up();               ///< local space up direction in the neutral position
    Vector3 forward = Vector3::Front();       ///< local space forward direction in the neutral position
    Vector3 steeringAxis = Vector3::Up();     ///< local space steering axis pointing away from ground
    Vector3 suspensionAxis = Vector3::Down(); ///< local space suspension direction pointing towards ground
    Entity target = Entity::Null();           ///< optional Entity to animate (can be (un)assigned freely without issuing an update notification)

    static auto MakeDisabled()       -> WheelMount { return WheelMount{.id = -1}; }
    auto IsEnabled()           const -> bool       { return id != -1; }
};

/** @brief Suspension settings for a wheel assembly. */
struct Suspension
{
    float minLength = 0.01f;           ///< minimum offset from wheel position [units: m, range: >= 0]
    float maxLength = 0.7f;            ///< maximum offset from wheel position [units: m, range: > minLength]
    SpringSettings spring{1.5f, 0.5f}; ///< suspension spring settings [range: frequency > 0, damping >= 0]
    float antiRollBarStiffness = 0.0f; ///< anti-roll bar spring stiffness; ignored for single-wheels; immutable after construction [units: N/m, range: >=0]

    static auto MakeDisabled() -> Suspension
    {
        return Suspension{0.0f, 0.0001f, SpringSettings{0.0f, 0.0f}, 0.0f};
    }
};

/**
 * @brief Differential settings for a wheel assembly.
 * @note Set ratio to 0 to disable power to the wheel(s).
 */
struct Differential
{
    float ratio = 3.42f;           ///< ratio between rotation speed of gear box and wheels (set to 0 for unpowered wheels) [range: (0, FLT_MAX)]
    float limitedSlipRatio = 1.4f; ///< ratio between max and min wheel speed controlling torque distribution [range: (1, FLT_MAX]]

    static auto MakeDisabled()       -> Differential { return Differential{.ratio = 0.0f}; }
    auto IsEnabled()           const -> bool         { return ratio != 0.0f; }
};

/**
 * @brief Settings describing a set of wheels, suspension, and differential attached to a Vehicle.
 * @note For single wheel attachments, either the left or right wheel may be used. At least one wheel must be enabled.
 */
struct WheelAssembly
{
    WheelMount leftWheel{};           ///< settings for the left wheel
    WheelMount rightWheel{};          ///< settings for the right wheel
    WheelSpec wheelSpec{};            ///< shared wheel settings
    Suspension suspension{};          ///< shared suspension settings
    Differential differential{};      ///< optional differential settings

    auto WheelCount() const -> size_t
    {
        return static_cast<size_t>(leftWheel.IsEnabled()) +
               static_cast<size_t>(rightWheel.IsEnabled());
    }

    auto IsPowered() const -> bool
    {
        return differential.IsEnabled();
    }
};

/** @brief Initialization info for a Vehicle. */
struct VehicleInfo
{
    VehicleOrientation orientation{};                            ///< vehicle orientation settings
    VehicleEngine engine{};                                      ///< vehicle engine settings
    VehicleTransmission transmission{};                          ///< vehicle transmission settings
    std::vector<WheelAssembly> wheelAssemblies{WheelAssembly{}}; ///< vehicle wheels (at least one wheel assembly with an enabled differential is required)
};

/** @brief A vehicle that can be added to a RigidBody. */
class Vehicle
{
    public:
        explicit Vehicle(VehicleInfo&& info,
                         VehicleHandle handle,
                         VehicleId id)
            : m_info{std::move(info)},
              m_handle{handle},
              m_id{id}
        {
        }

        /**
         * @name Vehicle Input
         * @note Vehicle input does not wake the owning RigidBody. The RigidBody can be awakened when needed, or
         *       RigidBodyFlags::DisableSleeping may be set.
         * 
         * Throttle values range from [-1, 1] indicating reverse/forward direction and throttle amount.
         * Steering values range from [-1, 1] indicating left/right steering angle and amount.
         * Brake values range from [0, 1] indicating brake amount.
         */
        auto GetThrottle()  const -> float;
        auto GetSteering()  const -> float;
        auto GetBrake()     const -> float;
        auto GetHandBrake() const -> float;
        void SetThrottle(float throttle);
        void SetSteering(float steering);
        void SetBrake(float brake);
        void SetHandBrake(float brake);
        void SetInput(float throttle, float steering, float brake, float handBrake);
        void HardStopEngine(); // zero out input, engine, and transmission state.

        /**
         * @name Vehicle Settings
         * @note Writes to these do not update internal state until one of the notification functions below is called.
         */
        auto GetVehicleInfo()       const -> const VehicleInfo&             { return m_info;                        }
        auto GetOrientation()       const -> const VehicleOrientation&      { return m_info.orientation;            }
        auto GetEngine()                  ->       VehicleEngine&           { return m_info.engine;                 }
        auto GetEngine()            const -> const VehicleEngine&           { return m_info.engine;                 }
        auto GetTransmission()            ->       VehicleTransmission&     { return m_info.transmission;           }
        auto GetTransmission()      const -> const VehicleTransmission&     { return m_info.transmission;           }
        auto GetWheelAssemblies()         -> std::span<WheelAssembly>       { return m_info.wheelAssemblies;        }
        auto GetWheelAssemblies()   const -> std::span<const WheelAssembly> { return m_info.wheelAssemblies;        }
        auto GetAssemblyCount()     const -> size_t                         { return m_info.wheelAssemblies.size(); }
        auto GetWheelCount()        const -> size_t;
        auto GetDifferentialCount() const -> size_t;

        /**
         * @name Commit Vehicle Settings Modifications
         * 
         * Notify the physics engine that vehicle settings have been modified and internal state should be rebuilt.
         * Notification isn't necessary when updating an animation target. The following restrictions apply:
         *   - Valid ranges must be respected.
         *   - A vehicle must always have at least one enabled wheel, differential, forward gear, and reverse gear.
         *   - VehicleOrientation reference axes are immutable.
         *   - Removing gears may force a shift to an adjacent gear.
         */
        void SetMaxRollAngle(float angle);
        void NotifyModifyEngine();
        void NotifyModifyTransmission();
        void NotifyModifyWheelAssembly(size_t assemblyIndex);
        void AddWheelAssembly(const WheelAssembly& info);
        void RemoveWheelAssembly(size_t assemblyIndex);

        /** @name Vehicle Enable State */
        void Enable(bool enabled);
        auto IsEnabled() const -> bool;

        /** @name Unique Identifiers */
        auto GetId()     const -> VehicleId     { return m_id;     }
        auto GetHandle() const -> VehicleHandle { return m_handle; }

    private:
        VehicleInfo m_info;
        VehicleHandle m_handle;
        VehicleId m_id;
};
} // namespace nc
