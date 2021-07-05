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
        float angularDrag = 0.05f;
        float restitution = 0.3f;
        float friction = 0.3f;
        float baumgarte = 0.4f;
        bool useGravity = true;
    };

    class PhysicsBody final : public ComponentBase
    {
        public:
            PhysicsBody(Entity entity, PhysicsProperties properties);

            void UpdateWorldInertia(Transform* transform);
            auto GetProperties() -> PhysicsProperties& { return m_properties; }
            auto GetInverseInertia() const -> DirectX::FXMMATRIX { return m_inertiaWorld; }

        private:
            PhysicsProperties m_properties;
            DirectX::XMMATRIX m_inertiaWorld;
            DirectX::XMVECTOR m_inertiaLocal;
    };

    // template<>
    // struct StoragePolicy<PhysicsBody>
    // {
    //     #ifdef NC_EDITOR_ENABLED
    //     using allow_trivial_destruction = std::false_type;
    //     #else
    //     using allow_trivial_destruction = std::true_type;
    //     #endif

    //     using sort_dense_storage_by_address = std::true_type;
    //     using requires_on_add_callback = std::true_type;
    //     using requires_on_remove_callback = std::true_type;
    // };

    #ifdef NC_EDITOR_ENABLED
    template<> void ComponentGuiElement<PhysicsBody>(PhysicsBody*);
    #endif
}