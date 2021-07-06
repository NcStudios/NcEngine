#pragma once

#include "component/Transform.h"

namespace nc
{
    struct PhysicsProperties
    {
        Vector3 velocity = Vector3::Zero();
        Vector3 angularVelocity = Vector3::Zero();
        Vector3 linearFreedom = Vector3::One();
        Vector3 angularFreedom = Vector3::One();
        float mass = 1.0f;
        float drag = 0.05f;
        float angularDrag = 0.1f;
        float restitution = 0.3f;
        float friction = 0.5f;
        float baumgarte = 0.3f;
        bool useGravity = true;
    };

    class PhysicsBody final : public ComponentBase
    {
        public:
            PhysicsBody(Entity entity, PhysicsProperties properties);

            void UpdateWorldInertia(Transform* transform);
            auto GetProperties() -> PhysicsProperties& { return m_properties; }
            auto GetInverseInertia() const -> DirectX::FXMMATRIX { return m_invInertiaWorld; }

        private:
            PhysicsProperties m_properties;
            DirectX::XMMATRIX m_invInertiaWorld;
            DirectX::XMMATRIX m_invInertiaLocal;
    };

    #ifdef NC_EDITOR_ENABLED
    template<> void ComponentGuiElement<PhysicsBody>(PhysicsBody*);
    #endif
}