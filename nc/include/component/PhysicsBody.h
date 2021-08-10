#pragma once

#include "component/Transform.h"

namespace nc
{
    /** Notes on physics bodies
     *  - A PhysicsBody must be added to an Entity with a Collider.
     *  - Static Entities will automatically have a mass of 0.
     *  - Translations and rotations can be resticted on a per-axis
     *    basis with linear/angularFreedom.
     */
    struct PhysicsProperties
    {
        float mass = 1.0f;        // Mass in kilograms (0 is infinite)
        float drag = 0.05f;       // Linear resistence
        float angularDrag = 0.1f; // Angular resistence
        float restitution = 0.3f; // Coefficient of restitution (collision elasticity) [0, 1]
        float friction = 0.5f;    // Coefficient of friction [0, 1]
        bool useGravity = true;   // Is the object affected by gravity
        bool kinematic  = false;  // Prevent forces and collisions from affecting the object
    };

    enum class IntegrationResult
    {
        Integrated, Ignored, PutToSleep
    };

    class PhysicsBody final : public ComponentBase
    {
        public:
            PhysicsBody(Entity entity, PhysicsProperties properties, Vector3 linearFreedom = Vector3::One(), Vector3 angularFreedom = Vector3::One());

            void ApplyImpulse(const Vector3& impulse);
            void ApplyImpulse(DirectX::FXMVECTOR impulse);
            void ApplyTorqueImpulse(const Vector3& torque);
            void ApplyTorqueImpulse(DirectX::FXMVECTOR torque);
            void ApplyVelocity(DirectX::FXMVECTOR delta);
            void ApplyVelocities(DirectX::FXMVECTOR velDelta, DirectX::FXMVECTOR angVelDelta);
            void UpdateWorldInertia(Transform* transform);
            auto Integrate(Transform* transform, float dt) -> IntegrationResult;
            void Wake() { m_framesAtThreshold = 0u; m_awake = true; }
            void Sleep() { m_awake = false; }

            auto GetVelocity() const -> DirectX::FXMVECTOR { return m_linearVelocity; }
            auto GetAngularVelocity() const -> DirectX::FXMVECTOR { return m_angularVelocity; }
            auto GetInverseMass() const -> float { return m_properties.mass; }
            auto GetFriction() const -> float { return m_properties.friction; }
            auto GetRestitution() const -> float { return m_properties.restitution; }
            auto GetInverseInertia() const -> DirectX::FXMMATRIX { return m_invInertiaWorld; }
            auto UseGravity() const -> bool { return m_properties.useGravity; }
            auto IsKinematic() const -> bool { return m_properties.kinematic; }
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
            friend void ComponentGuiElement<PhysicsBody>(PhysicsBody*);
            #endif
    };

    #ifdef NC_EDITOR_ENABLED
    template<> void ComponentGuiElement<PhysicsBody>(PhysicsBody*);
    #endif
}