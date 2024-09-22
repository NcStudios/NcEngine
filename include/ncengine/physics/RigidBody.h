/**
 * @file RigidBody.h
 * @copyright Jaremie Romer and McCallister Romer 2024
 */
#pragma once

#include "ncengine/ecs/Component.h"
#include "ncengine/ecs/Transform.h"
#include "ncengine/physics/Constraints.h"
#include "ncengine/physics/Shape.h"
#include "ncengine/utility/MatrixUtilities.h"

namespace nc::physics
{
struct ComponentContext;

/** @brief Handle to internal RigidBody state. */
using BodyHandle = void*;

/**
 * @brief Flags indicating allowed degrees of freedom of a RigidBody.
 * @note To restrict all motion, use BodyType::Static instead.
 */
struct DegreeOfFreedom
{
    using Type = uint8_t;

    static constexpr Type All          = 0b00111111;
    static constexpr Type TranslationX = 0b00000001;
    static constexpr Type TranslationY = 0b00000010;
    static constexpr Type TranslationZ = 0b00000100;
    static constexpr Type Translation  = 0b00000111;
    static constexpr Type RotationX    = 0b00001000;
    static constexpr Type RotationY    = 0b00010000;
    static constexpr Type RotationZ    = 0b00100000;
    static constexpr Type Rotation     = 0b00111000;
};

/** @brief Flags for configuring RigidBody behavior. */
struct RigidBodyFlags
{
    using Type = uint8_t;

    /** @brief Disable all flags. */
    static constexpr Type None = 0x0;

    /** @brief Disables the collision response for the body and raises trigger events instead of collision events. */
    static constexpr Type Trigger = 0x1;

    /** @brief Enable continuous collision detection on the body using a linear shape cast. (incompatible with Trigger) */
    static constexpr Type ContinuousDetection = 0x2;

    /** @brief Scale the body's shape using the associated Transform's scale.
     * 
     * For shapes with scaling restrictions, this may force updates to the Transform to prevent invalid shape scaling.
     * E.g. placing a sphere on an object with non-uniform Transform scaling will update the Transform to have uniform
     * scaling.
     */
    static constexpr Type IgnoreTransformScaling = 0x4;
};

/** @brief Determines movement and collision behavior of a RigidBody. */
enum class BodyType : uint8_t
{
    Dynamic,   ///< movable with velocities and forces; collides with all other bodies
    Kinematic, ///< movable only with velocities; collides with all other bodies
    Static     ///< non-movable; does not collide with other static bodies (required if Entity is static)
};

/** @brief Properties for initializing a RigidBody. */
struct RigidBodyInfo
{
    static constexpr auto maxGravityMultiplier = 100.0f;

    float friction = 0.2f;                                ///< friction of the body (range: [0, 1])
    float restitution = 0.0f;                             ///< elasticity of collision response (range: [0, 1])
    float linearDamping = 0.0f;                           ///< linear motion damping (range: [0, 1])
    float angularDamping = 0.0f;                          ///< angular motion damping (range: [0, 1])
    float gravityMultiplier = 1.0f;                       ///< amount of gravity applied to the body (range: [0, maxGravityMultiplier])
    BodyType type = BodyType::Dynamic;                    ///< set type of body (on a static Entity, this will be overwritten to BodyType::Static)
    DegreeOfFreedom::Type freedom = DegreeOfFreedom::All; ///< set degrees of freedom for the body
    RigidBodyFlags::Type flags = RigidBodyFlags::None;    ///< set flags for the body
};

/** @brief Component managing physics simulation properties. */
class RigidBody
{
    public:
        /** @name Special Member Functions */
        RigidBody(Entity self,
                  const Shape& shape = Shape::MakeBox(),
                  const RigidBodyInfo& info = RigidBodyInfo{})
            : m_self{self},
              m_shape{shape},
              m_info{info}
        {
            if (self.IsStatic())
            {
                m_info.type = BodyType::Static;
            }

            constexpr auto exclusiveFlags = RigidBodyFlags::Trigger | RigidBodyFlags::ContinuousDetection;
            if ((m_info.flags & exclusiveFlags) == exclusiveFlags)
            {
                m_info.flags &= ~RigidBodyFlags::ContinuousDetection;
            }
        }

        RigidBody(RigidBody&& other) noexcept
            : m_self{std::exchange(other.m_self, Entity::Null())},
              m_handle{std::exchange(other.m_handle, nullptr)},
              m_shape{other.m_shape},
              m_info{other.m_info}
        {
        }

        RigidBody& operator=(RigidBody&& other) noexcept
        {
            if (this != &other)
            {
                m_self = std::exchange(other.m_self, Entity::Null());
                m_handle = std::exchange(other.m_handle, nullptr);
                m_shape = other.m_shape;
                m_info = other.m_info;
            }

            return *this;
        }

        RigidBody(RigidBody&) = delete;
        RigidBody& operator=(RigidBody&) = delete;

        /** @name General Functions */
        auto GetEntity() const -> Entity { return m_self; }

        /**
         * @name BodyType Functions
         * @note BodyType::Static is forced if attached to a static Entity.
         */
        auto GetBodyType() const -> BodyType { return m_info.type; }
        void SetBodyType(BodyType type, bool wake = true);

        /** @name DegreeOfFreedom Functions */
        auto GetDegreesOfFreedom() const -> DegreeOfFreedom::Type { return m_info.freedom; }
        void SetDegreesOfFreedom(DegreeOfFreedom::Type dof);

        /** @name Shape Functions */
        auto GetShape() const -> const Shape& { return m_shape; }
        void SetShape(const Shape& shape, const Vector3& transformScale, bool wake = true); // todo: how to update transform?

        /** @name Simulation Properties */
        auto IsAwake() const -> bool;
        void SetAwakeState(bool wake);
        auto GetFriction() const -> float { return m_info.friction; }
        void SetFriction(float friction);
        auto GetRestitution() const -> float { return m_info.restitution; }
        void SetRestitution(float restitution);
        auto GetLinearDamping() const -> float { return m_info.linearDamping; }
        void SetLinearDamping(float damping); // can set?
        auto GetAngularDamping() const -> float { return m_info.angularDamping; }
        void SetAngularDamping(float damping); // can set?
        auto GetGravityMultiplier() const -> float { return m_info.gravityMultiplier; }
        void SetGravityMultiplier(float factor);

        /** @name RigidBodyFlags Functions */
        auto IsTrigger() const -> bool { return m_info.flags & RigidBodyFlags::Trigger; }
        void SetTrigger(bool value);
        auto UseContinuousDetection() const -> bool { return m_info.flags & RigidBodyFlags::ContinuousDetection; }
        void UseContinuousDetection(bool value);
        auto ScalesWithTransform() const -> bool { return !IgnoreTransformScaling(); }
        auto IgnoreTransformScaling() const -> bool { return m_info.flags & RigidBodyFlags::IgnoreTransformScaling; }
        void IgnoreTransformScaling(bool value);
        /**
         * @name Velocity Functions
         * @note Requires BodyType::Dynamic or BodyType::Kinematic
         */
        auto GetLinearVelocity() const -> Vector3;
        void SetLinearVelocity(const Vector3& velocity);
        void AddLinearVelocity(const Vector3& velocity);
        auto GetAngularVelocity() const -> Vector3;
        void SetAngularVelocity(const Vector3& velocity);
        void SetVelocities(const Vector3& linearVelocity, const Vector3& angularVelocity);
        void AddVelocities(const Vector3& linearVelocity, const Vector3& angularVelocity);

        /**
         * @name Force/Impulse Functions
         * @note Requires BodyType::Dynamic
         */
        void AddForce(const Vector3& force);
        void AddForceAt(const Vector3& force, const Vector3& point);
        void AddTorque(const Vector3& torque);
        void AddImpulse(const Vector3& impulse);
        void AddImpulseAt(const Vector3& impulse, const Vector3& point);
        void AddAngularImpulse(const Vector3& impulse);

        /** @name Constraint Functions */
        /** @brief Add a Constraint between the RigidBody and another. */
        auto AddConstraint(const ConstraintInfo& createInfo, const RigidBody& otherBody) -> Constraint&;

        /** @brief Add a Constraint between the RigidBody and the world. */
        auto AddConstraint(const ConstraintInfo& createInfo) -> Constraint&;

        /** @brief Remove a constraint from the RigidBody. */
        void RemoveConstraint(ConstraintId constraintId);

        /** @brief View all of the constraints attached to the RigidBody. */
        auto GetConstraints() -> std::span<Constraint>;

        /**
         * @name Simulated Body Functions
         * @note Prefer using simulated body functions over the Transform equivalents for \ref Entity "entities" with a RigidBody.
         * 
         * Simulated body functions synchronize updating of properties shared between a Transform and RigidBody. They should only
         * be used when strictly necessary, as directly modifying these properties can cause undesirable behavior in the simulation
         * (e.g. when repositioning one body inside of another).
         * @{
         */
        /** @brief Set the position of an object's Transform and RigidBody. */
        void SetSimulatedBodyPosition(Transform& transform, const Vector3& position, bool wake = true);

        /** @brief Set the rotation of an object's Transform and RigidBody. */
        void SetSimulatedBodyRotation(Transform& transform, const Quaternion& rotation, bool wake = true);

        /**
         * @brief Set the scale of an object's Transform and RigidBody.
         * 
         * The actual applied scale is returned and may differ from the requested value, depending on scaling requirements of the
         * RigidBody Shape. If the body does not have ScaleWithTransform set, only the Transform will be modified.
         */
        auto SetSimulatedBodyScale(Transform& transform, const Vector3& scale, bool wake = true) -> Vector3;
        /** @} */ // End Simulated Body Functions

        /** @cond internal */
        auto IsInitialized() const noexcept -> bool { return m_handle; }
        auto GetHandle() const -> BodyHandle { return m_handle; }
        void SetHandle(BodyHandle handle) { m_handle = handle; }
        static void SetContext(ComponentContext* ctx) { s_ctx = ctx; }
        /** @endcond */

    private:
        inline static ComponentContext* s_ctx = nullptr;

        Entity m_self;
        BodyHandle m_handle = nullptr;
        Shape m_shape;
        RigidBodyInfo m_info;
};
} // namespace nc::physics

namespace nc
{
template<>
struct StoragePolicy<physics::RigidBody> : DefaultStoragePolicy
{
    static constexpr bool EnableOnAddCallbacks = true;
    static constexpr bool EnableOnCommitCallbacks = false;
    static constexpr bool EnableOnRemoveCallbacks = true;
};
} // namespace nc
