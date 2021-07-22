#pragma once

#include "component/Transform.h"

namespace nc
{
    struct PhysicsProperties
    {
        Vector3 velocity = Vector3::Zero();        // Current linear velocity
        Vector3 angularVelocity = Vector3::Zero(); // Current angular velocity
        Vector3 linearFreedom = Vector3::One();    // Per-axis translation restriction (use 0 to lock)
        Vector3 angularFreedom = Vector3::One();   // Per-axis rotation restriction (use 0 to lock)
        float mass = 1.0f;                         // Mass in kilograms (0 is infinite)
        float drag = 0.05f;                        // Linear resistence
        float angularDrag = 0.1f;                  // Angular resistence
        float restitution = 0.3f;                  // Coefficient of restitution (collision elasticity) [0, 1]
        float friction = 0.5f;                     // Coefficient of friction [0, 1]
        float baumgarte = 0.4f;                    // Position correction factor to apply on collision [0, 1]
        bool useGravity = true;                    // Is the object affected by gravity
        bool kinematic  = false;                   // Prevent forces and collisions from affecting the object
    };

    class PhysicsBody final : public ComponentBase
    {
        public:
            PhysicsBody(Entity entity, PhysicsProperties properties);

            void UpdateWorldInertia(Transform* transform);
            auto GetProperties() -> PhysicsProperties& { return m_properties; }
            auto GetInverseInertia() const -> DirectX::FXMMATRIX { return m_invInertiaWorld; }
            bool IsKinematic() const { return m_properties.kinematic; }
            
        private:
            PhysicsProperties m_properties;
            DirectX::XMMATRIX m_invInertiaWorld;
            Vector3 m_invInertiaLocal;
    };

    #ifdef NC_EDITOR_ENABLED
    template<> void ComponentGuiElement<PhysicsBody>(PhysicsBody*);
    #endif
}