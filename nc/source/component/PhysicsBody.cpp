#include "component/PhysicsBody.h"
#include "Ecs.h"

namespace
{
    DirectX::XMMATRIX CreateInverseInertiaTensor(const nc::Vector3& scale, float mass, nc::ColliderType type)
    {
        float iX, iY, iZ;

        switch(type)
        {
            case nc::ColliderType::Box:
            {
                auto m = mass / 12.0f;
                auto squareScale = nc::HadamardProduct(scale, scale);
                iX = m * (squareScale.y + squareScale.z);
                iY = m * (squareScale.x + squareScale.z);
                iZ = m * (squareScale.x + squareScale.y);
                break;
            }
            case nc::ColliderType::Capsule:
            {
                /** @todo This is for a cylinder. */
                auto m = mass / 12.0f;
                auto r = scale.x * 0.5f;
                auto h = scale.y * 2.0f;
                iX = iY = m * (3.0f * r * r + h * h);
                iZ = m * r * r * 0.5f;
                break;
            }
            case nc::ColliderType::Sphere:
            {
                float radius = scale.x * 0.5f;
                iX = iY = iZ = (2.0f / 3.0f) * mass * radius * radius;
                break;
            }
            case nc::ColliderType::Hull:
            {
                /** @todo Need to compute these in preprocessing. Use sphere for now. */
                float sqRadius = scale.x * scale.x;
                iX = iY = iZ = (2.0f / 3.0f) * mass * sqRadius;
                break;
            }
            default:
            {
                throw std::runtime_error("GetInverseInertiaTensor - Unknown ColliderType");
            }
        }

        return DirectX::XMMatrixInverse(nullptr, DirectX::XMMatrixScaling(iX, iY, iZ));
    }
}

#define NC_PHYSICS_DEBUGGING 1

namespace nc
{
    PhysicsBody::PhysicsBody(Entity entity, PhysicsProperties properties)
        : ComponentBase{entity},
          m_properties{properties},
          m_invInertiaWorld{},
          m_invInertiaLocal{}
    {
        auto* registry = ActiveRegistry();
        auto* collider = registry->Get<Collider>(entity);
        
        if(!collider)
            throw std::runtime_error("PhysicsBody added to Entity with no Collider");

        if(EntityUtils::IsStatic(entity))
        {
            m_properties.mass = 0.0f;
        }

        if(m_properties.mass == 0.0f)
        {
            m_invInertiaLocal = DirectX::XMMatrixScaling(0.0f, 0.0f, 0.0f);
            return;
        }

        auto colliderScale = collider->GetInfo().scale;
        auto transformScale = registry->Get<Transform>(entity)->GetScale();
        auto totalScale = HadamardProduct(transformScale, colliderScale);
        m_invInertiaLocal = CreateInverseInertiaTensor(totalScale, m_properties.mass, collider->GetType());
    }

    void PhysicsBody::UpdateWorldInertia(Transform* transform)
    {
        auto rot_v = transform->GetRotationXM();
        auto rot_m = DirectX::XMMatrixRotationQuaternion(rot_v);
        m_invInertiaWorld = rot_m * m_invInertiaLocal * XMMatrixTranspose(rot_m);
    }

    #ifdef NC_EDITOR_ENABLED
    template<> void ComponentGuiElement<PhysicsBody>(PhysicsBody*)
    {
    }
    #endif
}