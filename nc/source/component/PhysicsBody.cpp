#include "component/PhysicsBody.h"
#include "Ecs.h"
#include "physics/PhysicsConstants.h"

namespace
{
    using namespace nc;

    const auto VelocitySleepThreshold = DirectX::XMVectorSet(physics::SleepEpsilon, physics::SleepEpsilon, physics::SleepEpsilon, 0.0f);

    Vector3 CreateInverseInertiaTensor(const Vector3& scale, float mass, ColliderType type)
    {
        float iX, iY, iZ;

        switch(type)
        {
            case ColliderType::Box:
            {
                auto m = mass / 12.0f;
                auto squareScale = nc::HadamardProduct(scale, scale);
                iX = m * (squareScale.y + squareScale.z);
                iY = m * (squareScale.x + squareScale.z);
                iZ = m * (squareScale.x + squareScale.y);
                break;
            }
            case ColliderType::Capsule:
            {
                /** @todo This is for a cylinder. */
                auto m = mass / 12.0f;
                auto r = scale.x * 0.5f;
                auto h = scale.y * 2.0f;
                iX = iY = m * (3.0f * r * r + h * h);
                iZ = m * r * r * 0.5f;
                break;
            }
            case ColliderType::Sphere:
            {
                float radius = scale.x * 0.5f;
                iX = iY = iZ = (2.0f / 3.0f) * mass * radius * radius;
                break;
            }
            case ColliderType::Hull:
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

        return Vector3{1.0f / iX, 1.0f / iY, 1.0f / iZ};
    }
}

namespace nc
{
    PhysicsBody::PhysicsBody(Entity entity, PhysicsProperties properties, Vector3 linearFreedom, Vector3 angularFreedom)
        : ComponentBase{entity},
          m_properties{properties},
          m_velocity{},
          m_invInertiaWorld{},
          m_invInertiaLocal{},
          m_framesAtThreshold{0u},
          m_awake{true}
    {
        auto* registry = ActiveRegistry();
        auto* collider = registry->Get<Collider>(entity);
        
        if(!collider)
            throw std::runtime_error("PhysicsBody added to Entity with no Collider");

        if(EntityUtils::IsStatic(entity))
        {
            m_properties.mass = 0.0f;
        }

        m_velocity.r[2] = DirectX::XMLoadVector3(&linearFreedom);
        m_velocity.r[3] = DirectX::XMLoadVector3(&angularFreedom);

        if(m_properties.mass == 0.0f)
        {
            m_invInertiaLocal = Vector3::Zero();
            return;
        }

        auto colliderScale = collider->GetInfo().scale;
        auto transformScale = registry->Get<Transform>(entity)->GetScale();
        auto totalScale = HadamardProduct(transformScale, colliderScale);
        m_invInertiaLocal = CreateInverseInertiaTensor(totalScale, m_properties.mass, collider->GetType());
    }

    void PhysicsBody::UpdateWorldInertia(Transform* transform)
    {
        if(EntityUtils::IsStatic(GetParentEntity()))
            return;

        auto rot_v = transform->GetRotationXM();
        auto rot_m = DirectX::XMMatrixRotationQuaternion(rot_v);
        auto invInertiaLocalMatrix = DirectX::XMMatrixScaling(m_invInertiaLocal.x, m_invInertiaLocal.y, m_invInertiaLocal.z);
        m_invInertiaWorld = rot_m * invInertiaLocalMatrix * DirectX::XMMatrixTranspose(rot_m);
    }

    void PhysicsBody::UpdateVelocities(DirectX::FXMVECTOR velDelta, DirectX::FXMVECTOR angVelDelta)
    {
        if(m_properties.isKinematic || EntityUtils::IsStatic(GetParentEntity()))
            return;
        
        m_velocity.r[0] += velDelta * m_velocity.r[2];
        m_velocity.r[1] += angVelDelta * m_velocity.r[3];
    }

    void PhysicsBody::UpdateVelocity(DirectX::FXMVECTOR delta)
    {
        if(m_properties.isKinematic || EntityUtils::IsStatic(GetParentEntity()))
            return;
        
        m_velocity.r[0] += delta * m_velocity.r[2];
    }

    IntegrationResult PhysicsBody::Integrate(Transform* transform, float dt)
    {
        if(m_properties.isKinematic || !m_awake)
            return IntegrationResult::Ignored;
        
        auto linearDragFactor = pow(1.0f - m_properties.drag, dt);
        auto angularDragFactor = pow(1.0f - m_properties.angularDrag, dt);
        m_velocity.r[0] = DirectX::XMVectorScale(m_velocity.r[0], linearDragFactor);
        m_velocity.r[1] = DirectX::XMVectorScale(m_velocity.r[1], angularDragFactor);

        auto velSquareMag = DirectX::XMVector3LengthSq(m_velocity.r[0]) + DirectX::XMVector3LengthSq(m_velocity.r[1]);

        if(DirectX::XMVector3Less(velSquareMag, VelocitySleepThreshold))
        {
            if(++m_framesAtThreshold >= physics::FramesUntilSleep)
            {
                m_velocity.r[0] = DirectX::g_XMZero;
                m_velocity.r[1] = DirectX::g_XMZero;
                m_awake = false;

                return IntegrationResult::PutToSleep;
            }
        }
        else
        {
            m_framesAtThreshold = 0u;
        }

        auto rotQuat = DirectX::XMVectorScale(m_velocity.r[1], dt);
        rotQuat = DirectX::XMQuaternionRotationRollPitchYawFromVector(rotQuat);

        transform->Translate(DirectX::XMVectorScale(m_velocity.r[0], dt));
        transform->Rotate(rotQuat);
        return IntegrationResult::Integrated;
    }

    #ifdef NC_EDITOR_ENABLED
    template<> void ComponentGuiElement<PhysicsBody>(PhysicsBody*)
    {
    }
    #endif
}