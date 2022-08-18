#pragma once

#include "ecs/Transform.h"

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
        PhysicsBody(Entity entity, PhysicsProperties properties, Vector3 linearFreedom = Vector3::One(), Vector3 angularFreedom = Vector3::One());

        void ApplyImpulse(const Vector3& impulse);
        void ApplyImpulse(DirectX::FXMVECTOR impulse);
        void ApplyTorqueImpulse(const Vector3& torque);
        void ApplyTorqueImpulse(DirectX::FXMVECTOR torque);
        void ApplyVelocity(DirectX::FXMVECTOR delta);
        void ApplyVelocities(DirectX::FXMVECTOR velDelta, DirectX::FXMVECTOR angVelDelta);
        void UpdateWorldInertia(const Transform* transform);
        auto Integrate(Transform* transform, float dt) -> IntegrationResult;
        void Wake() { m_framesAtThreshold = 0u; m_awake = true; }
        void Sleep() { m_awake = false; }

        auto GetVelocity() const -> DirectX::FXMVECTOR { return m_linearVelocity; }
        auto GetAngularVelocity() const -> DirectX::FXMVECTOR { return m_angularVelocity; }
        auto GetInverseMass() const -> float { return m_properties.mass; }
        auto GetDrag() const -> float { return m_properties.drag; }
        auto GetAngularDrag() const -> float { return m_properties.angularDrag; }
        auto GetFriction() const -> float { return m_properties.friction; }
        auto GetRestitution() const -> float { return m_properties.restitution; }
        auto GetInverseInertia() const -> DirectX::FXMMATRIX { return m_invInertiaWorld; }
        auto UseGravity() const -> bool { return m_properties.useGravity; }
        auto IsKinematic() const -> bool { return m_properties.isKinematic; }
        auto IsAwake() const -> bool { return m_awake; }

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

        #ifdef NC_EDITOR_ENABLED
        friend void nc::ComponentGuiElement<PhysicsBody>(PhysicsBody*);
        #endif
};
} // namespace nc::physics

namespace nc
{
#ifdef NC_EDITOR_ENABLED
template<> void ComponentGuiElement<physics::PhysicsBody>(physics::PhysicsBody*);
#endif
}
