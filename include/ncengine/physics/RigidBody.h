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

    /** @brief Default flag values. */
    static constexpr Type Default = ScaleWithTransform;
};

/** @brief Determines movement and collision behavior of a RigidBody. */
enum class BodyType : uint8_t
{
    Dynamic,  // movable with velocities and forces; collides with all other bodies
    Static,   // non-movable; does not collide with other static bodies
    Kinematic // movable only with velocities; collides with all other bodies
};

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

/** @brief Component managing physics simulation properties. */
class RigidBody
{
    public:
        RigidBody(Entity self,
                  const Shape& shape = Shape::MakeBox(),
                  BodyType bodyType = BodyType::Dynamic,
                  RigidBodyFlags::Type flags = RigidBodyFlags::Default)
            : m_self{self},
              m_shape{shape},
              m_bodyType{bodyType},
              m_flags{flags}
        {
        }

        RigidBody(RigidBody&& other) noexcept
            : m_self{std::exchange(other.m_self, Entity::Null())},
              m_handle{std::exchange(other.m_handle, nullptr)},
              m_ctx{std::exchange(other.m_ctx, nullptr)},
              m_shape{other.m_shape},
              m_bodyType{other.m_bodyType},
              m_flags{other.m_flags}
        {
        }

        RigidBody& operator=(RigidBody&& other) noexcept
        {
            m_self = std::exchange(other.m_self, Entity::Null());
            m_handle = std::exchange(other.m_handle, nullptr);
            m_ctx = std::exchange(other.m_ctx, nullptr);
            m_shape = other.m_shape;
            m_bodyType = other.m_bodyType;
            m_flags = other.m_flags;
            return *this;
        }

        ~RigidBody() noexcept;

        RigidBody(RigidBody&) = delete;
        RigidBody& operator=(RigidBody&) = delete;

        auto GetEntity() const -> Entity { return m_self; }
        auto GetShape() const -> const Shape& { return m_shape; }
        auto ScalesWithTransform() const -> bool { return m_flags & RigidBodyFlags::ScaleWithTransform; }
        auto GetBodyType() const -> BodyType { return m_bodyType; }
        auto GetHandle() const -> BodyHandle { return m_handle; }
        auto IsAwake() const -> bool;

        void AddImpulse(const Vector3& impulse);
        void AddTorque(const Vector3& torque);

        auto IsInitialized() const noexcept -> bool { return m_handle; }
        void SetPosition(const Vector3& position, bool wake = true);
        void SetRotation(const Quaternion& rotation, bool wake = true);
        auto SetScale(const Vector3& scale, bool wake = true) -> Vector3;

    private:
        friend class NcPhysicsImpl2;

        Entity m_self;
        BodyHandle m_handle = nullptr;
        ComponentContext* m_ctx = nullptr;
        Shape m_shape;
        BodyType m_bodyType;
        RigidBodyFlags::Type m_flags;

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
