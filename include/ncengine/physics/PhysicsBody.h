#pragma once

#include "ncengine/ecs/Transform.h"

namespace nc::physics
{
struct PhysicsProperties
{
    float mass = 1.0f;        // Mass in kilograms (0 is infinite)
    float drag = 0.05f;       // Linear resistance
    float angularDrag = 0.1f; // Angular resistence
    float restitution = 0.3f; // Coefficient of restitution (collision elasticity) [0, 1]
    float friction = 0.5f;    // Coefficient of friction [0, 1]
    bool useGravity = true;   // Is the object affected by gravity
    bool isKinematic  = false;  // Prevent forces and collisions from affecting the object
};

enum class IntegrationResult
{
    Integrated, Ignored, PutToSleep
};

class PhysicsBody final : public ComponentBase
{
    NC_ENABLE_IN_EDITOR(PhysicsBody)
    
    public:
        PhysicsBody(Entity entity,
                    PhysicsProperties properties = PhysicsProperties{},
                    Vector3 linearFreedom = Vector3::One(),
                    Vector3 angularFreedom = Vector3::One());

        void SetProperties(const PhysicsProperties& properties);
        void SetLinearFreedom(const Vector3& linearFreedom);
        void SetAngularFreedom(const Vector3& angularFreedom);

        void ApplyImpulse(const Vector3& impulse);
        void ApplyImpulse(DirectX::FXMVECTOR impulse);
        void ApplyTorqueImpulse(const Vector3& torque);
        void ApplyTorqueImpulse(DirectX::FXMVECTOR torque);
        void ApplyVelocity(DirectX::FXMVECTOR delta);
        void ApplyVelocities(DirectX::FXMVECTOR velDelta, DirectX::FXMVECTOR angVelDelta);
        void UpdateWorldInertia(const Transform* transform);
        auto Integrate(Transform* transform, float dt) -> IntegrationResult;
        void Wake() noexcept { m_framesAtThreshold = 0u; m_awake = true; }
        void Sleep() noexcept { m_awake = false; }

        auto GetProperties() const noexcept -> const PhysicsProperties& { return m_properties; }
        auto GetVelocity() const noexcept -> DirectX::FXMVECTOR { return m_linearVelocity; }
        auto GetAngularVelocity() const noexcept -> DirectX::FXMVECTOR { return m_angularVelocity; }
        auto GetInverseMass() const noexcept -> float { return m_properties.mass; }
        auto GetDrag() const noexcept -> float { return m_properties.drag; }
        auto GetAngularDrag() const noexcept -> float { return m_properties.angularDrag; }
        auto GetFriction() const noexcept -> float { return m_properties.friction; }
        auto GetRestitution() const noexcept -> float { return m_properties.restitution; }
        auto GetInverseInertia() const noexcept -> DirectX::FXMMATRIX { return m_invInertiaWorld; }
        auto UseGravity() const noexcept -> bool { return m_properties.useGravity; }
        auto IsKinematic() const noexcept -> bool { return m_properties.isKinematic; }
        auto IsAwake() const noexcept -> bool { return m_awake; }

    private:
        PhysicsProperties m_properties;
        DirectX::XMVECTOR m_linearVelocity;
        DirectX::XMVECTOR m_angularVelocity;
        DirectX::XMVECTOR m_linearFreedom;
        DirectX::XMVECTOR m_angularFreedom;
        DirectX::XMMATRIX m_invInertiaWorld;
        Vector3 m_invInertiaLocal;
        uint8_t m_framesAtThreshold;
        bool m_awake;
};
} // namespace nc::physics
