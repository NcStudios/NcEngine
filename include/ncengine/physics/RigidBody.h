#pragma once

#include "ncengine/ecs/Component.h"

#include "DirectXMath.h"

namespace nc::physics
{
using BodyHandle = void*;
using BodyInterface = void*;

enum class Shape : uint8_t
{
    Box,
    Sphere
};

enum class BodyType : uint8_t
{
    Dynamic,
    Static,
    Kinematic
};

class RigidBody
{
    public:
        RigidBody(Entity self, Shape shape, BodyType bodyType)
            : m_self{self},
              m_shape{shape},
              m_bodyType{bodyType}
        {
        }

        RigidBody(RigidBody&& other) noexcept
            : m_self{std::exchange(other.m_self, Entity::Null())},
              m_handle{std::exchange(other.m_handle, nullptr)},
              m_interface{std::exchange(other.m_interface, nullptr)},
              m_shape{other.m_shape},
              m_bodyType{other.m_bodyType}
        {
        }

        RigidBody& operator=(RigidBody&& other) noexcept
        {
            m_self = std::exchange(other.m_self, Entity::Null());
            m_handle = std::exchange(other.m_handle, nullptr);
            m_interface = std::exchange(other.m_interface, nullptr);
            m_shape = other.m_shape;
            m_bodyType = other.m_bodyType;
            return *this;
        }

        ~RigidBody() noexcept;

        RigidBody(RigidBody&) = delete;
        RigidBody& operator=(RigidBody&) = delete;

        auto GetEntity() const -> Entity { return m_self; }
        auto GetShape() const -> Shape { return m_shape; }
        auto GetBodyType() const -> BodyType { return m_bodyType; }
        auto GetHandle() const -> BodyHandle { return m_handle; }

        void AddImpulse(const Vector3& impulse);
        void AddTorque(const Vector3& torque);

        void Init(BodyHandle handle, BodyInterface interface);

    private:
        Entity m_self;
        BodyHandle m_handle = nullptr;
        BodyInterface m_interface = nullptr;
        Shape m_shape;
        BodyType m_bodyType;
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
