/**
 * @file RigidBody.h
 * @copyright Jaremie Romer and McCallister Romer 2024
 */
#pragma once

#include "Shape.h"
#include "ncengine/ecs/Component.h"
#include "ncengine/ecs/Transform.h"
#include "ncengine/utility/MatrixUtilities.h"

namespace nc::physics
{
struct ComponentContext;
class NcPhysicsImpl2;
class RigidBody;

/** @brief Handle to internal RigidBody state. */
using BodyHandle = void*;

/**
 * @defgroup SimulatedBodyFunctions Simulated Body Functions
 * @note Prefer using simulated body functions over the Transform equivalents for \ref Entity "entities" with a RigidBody.
 * 
 * Simulated body functions synchronize updating of properties shared between a Transform and RigidBody. They should only
 * be used when strictly necessary, as directly modifying these properties can cause undesirable behavior in the simulation
 * (e.g. when repositioning one body inside of another).
 *
 * @{
 */

/** @brief Set the position of an object's Transform and RigidBody. */
void SetSimulatedBodyPosition(Transform& transform,
                              RigidBody& rigidBody,
                              const Vector3& position,
                              bool wake = true);

/** @brief Set the rotation of an object's Transform and RigidBody. */
void SetSimulatedBodyRotation(Transform& transform,
                              RigidBody& rigidBody,
                              const Quaternion& rotation,
                              bool wake = true);

/**
 * @brief Set the scale of an object's Transform and RigidBody.
 * 
 * The actual applied scale is returned and may differ from the requested value, depending on scaling requirements of the
 * RigidBody Shape. If the body does not have ScaleWithTransform set, only the Transform will be modified - in this case,
 * RigidBody::SetScale() may be used to update the body independently.
 */
auto SetSimulatedBodyScale(Transform& transform,
                           RigidBody& rigidBody,
                           const Vector3& scale,
                           bool wake = true) -> Vector3;
/** @} */ // End SimulatedBodyFunctions

/** @brief Flags for configuring RigidBody behavior. */
struct RigidBodyFlags
{
    using Type = uint8_t;

    /** @brief Disable all flags. */
    static constexpr Type None = 0x0;

    /** @brief Scale the body's shape using the associated Transform's scale.
     * 
     * For shapes with scaling restrictions, this may force updates to the Transform to prevent invalid shape scaling.
     * E.g. placing a sphere on an object with non-uniform Transform scaling will update the Transform to have uniform
     * scaling.
     */
    static constexpr Type ScaleWithTransform = 0x1;

    /** @brief Enable continuous collision detection on the body using a linear shape cast. */
    static constexpr Type ContinuousDetection = 0x2;

    /** @brief Default flag values. */
    static constexpr Type Default = ScaleWithTransform;
};

/** @brief Determines movement and collision behavior of a RigidBody. */
enum class BodyType : uint8_t
{
    Dynamic,   // movable with velocities and forces; collides with all other bodies
    Kinematic, // movable only with velocities; collides with all other bodies
    Static     // non-movable; does not collide with other static bodies
};

/** @brief Properties for initializing a RigidBody. */
struct RigidBodyInfo
{
    float friction = 0.2f;          // [0, 1]
    float restitution = 0.0f;       // [0, 1]
    float linearDamping = 0.0f;     // [0, 1]
    float angularDamping = 0.0f;    // [0, 1]
    float gravityMultiplier = 1.0f; // [0, 1]
    BodyType type = BodyType::Dynamic;
    RigidBodyFlags::Type flags = RigidBodyFlags::Default;
};

/** @brief Component managing physics simulation properties. */
class RigidBody
{
    public:
        /** @name Special member functions */
        /** @{ */
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
        }

        RigidBody(RigidBody&& other) noexcept
            : m_self{std::exchange(other.m_self, Entity::Null())},
              m_handle{std::exchange(other.m_handle, nullptr)},
              m_ctx{std::exchange(other.m_ctx, nullptr)},
              m_shape{other.m_shape},
              m_info{other.m_info}
        {
        }

        RigidBody& operator=(RigidBody&& other) noexcept
        {
            m_self = std::exchange(other.m_self, Entity::Null());
            m_handle = std::exchange(other.m_handle, nullptr);
            m_ctx = std::exchange(other.m_ctx, nullptr);
            m_shape = other.m_shape;
            m_info = other.m_info;
            return *this;
        }

        ~RigidBody() noexcept;

        RigidBody(RigidBody&) = delete;
        RigidBody& operator=(RigidBody&) = delete;
        /** @} */

        /** @name Handle functions */
        auto GetEntity() const -> Entity { return m_self; }
        auto GetHandle() const -> BodyHandle { return m_handle; }

        /** 
         * @name BodyType functions
         * @note BodyType::Static is forced if attached to a static Entity.
         */
        auto GetBodyType() const -> BodyType { return m_info.type; }
        void SetBodyType(BodyType type);

        /** @name Shape functions */
        auto GetShape() const -> const Shape& { return m_shape; }
        void SetShape(const Shape& shape, const Vector3& transformScale);

        /** @name General physics property functions */
        auto GetAwakeState() const -> bool;
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

        /** @name RigidBodyFlags functions */
        auto ScalesWithTransform() const -> bool { return m_info.flags & RigidBodyFlags::ScaleWithTransform; }
        void ScalesWithTransform(bool value);
        auto UseContinuousDetection() const -> bool { return m_info.flags & RigidBodyFlags::ContinuousDetection; }
        void UseContinuousDetection(bool value);

        /** @name Force/Impulse functions - requires a dynamic body */
        void AddImpulse(const Vector3& impulse);
        void AddTorque(const Vector3& torque);

        /** @name Spatial body property functions */
        void SetBodyPosition(const Vector3& position, bool wake = true);
        void SetBodyRotation(const Quaternion& rotation, bool wake = true);
        auto SetBodyScale(const Vector3& previousScale, const Vector3& newScale, bool wake = true) -> Vector3;

        /** @cond internal */
        auto IsInitialized() const noexcept -> bool { return m_handle; }
        /** @endcond */

    private:
        friend class NcPhysicsImpl2;

        Entity m_self;
        BodyHandle m_handle = nullptr;
        ComponentContext* m_ctx = nullptr;
        Shape m_shape;
        RigidBodyInfo m_info;

        void SetContext(BodyHandle handle, ComponentContext* ctx);
};
} // namespace nc::physics

namespace nc
{
template<>
struct StoragePolicy<physics::RigidBody> : DefaultStoragePolicy
{
    static constexpr bool EnableOnAddCallbacks = true;
    static constexpr bool EnableOnCommitCallbacks = false;
    static constexpr bool EnableOnRemoveCallbacks = false;
};
} // namespace nc
