#include "component/PhysicsBody.h"
#include "Ecs.h"

#include <iostream>

namespace nc
{
    PhysicsBody::PhysicsBody(Entity entity, PhysicsProperties properties)
        : ComponentBase{entity},
            m_properties{properties},
            m_inertiaWorld{},
            m_inertiaLocal{}
    {
        /** @todo Maybe PhysicsSystem has Soa and tensors can live there. */
        auto* registry = ActiveRegistry();
        auto transformScale = registry->Get<Transform>(entity)->GetScale();
        auto colliderScale = registry->Get<Collider>(entity)->GetInfo().scale;
        
        auto m = properties.mass / 12.0f;
        auto totalScale = HadamardProduct(transformScale, colliderScale);
        auto squareDimensions = m * HadamardProduct(totalScale, totalScale);
        auto iX = squareDimensions.y + squareDimensions.z;
        auto iY = squareDimensions.x + squareDimensions.z;
        auto iZ = squareDimensions.x + squareDimensions.y;
        m_inertiaLocal = DirectX::XMVectorSet(iX, iY, iZ, 0.0f);


        //auto iv = properties.mass * (1.0f / 6.0f);
        //m_inertiaLocal = DirectX::XMVectorSet(iv, iv, iv, 0.0f);
    }

    // void PhysicsBody::UpdateWorldInertia(Transform* transform)
    // {
    //     auto rotation_v = transform->GetRotationXM();
    //     auto rotation_m = DirectX::XMMatrixRotationQuaternion(rotation_v);
    //     m_inertiaWorld = rotation_m;
    //     m_inertiaWorld.r[0] *= m_inertiaLocal;
    //     m_inertiaWorld.r[1] *= m_inertiaLocal;
    //     m_inertiaWorld.r[2] *= m_inertiaLocal;
    //     m_inertiaWorld.r[3] *= m_inertiaLocal;
    //     m_inertiaWorld *= DirectX::XMMatrixTranspose(rotation_m);
    // }

    void PhysicsBody::UpdateWorldInertia(Transform* transform)
    {
        // auto iv = m_properties.mass * (2.0f / 6.0f);
        // auto inertia = DirectX::XMMatrixSet
        // (
        //     iv,  0.0f, 0.0f, 0.0f,
        //     0.0f, iv,  0.0f, 0.0f,
        //     0.0f, 0.0f, iv,  0.0f,
        //     0.0f, 0.0f, 0.0f,  0.0f
        // );

        auto inertia = DirectX::XMMatrixScalingFromVector(m_inertiaLocal);

        auto rot_v = transform->GetRotationXM();
        auto rot_m = DirectX::XMMatrixRotationQuaternion(rot_v);
        m_inertiaWorld = rot_m * inertia * XMMatrixTranspose(rot_m);
    }

    #ifdef NC_EDITOR_ENABLED
    template<> void ComponentGuiElement<PhysicsBody>(PhysicsBody*)
    {
    }
    #endif
}