/**
 * @file PhysicsBody.h
 * @copyright Jaremie Romer and McCallister Romer 2024
 */
#pragma once

#include "ncengine/ecs/Transform.h"

namespace nc::physics
{
struct PhysicsProperties
{
    float mass = 1.0f;        // Mass in kilograms (0 is infinite)
    float drag = 0.05f;       // Linear resistance
    float angularDrag = 0.1f; // Angular resistence
    bool useGravity = true;   // Is the object affected by gravity
    bool isKinematic  = false;  // Prevent forces and collisions from affecting the object
};

enum class IntegrationResult
{
    Integrated, Ignored, PutToSleep
};

class Collider;

class PhysicsBody
{
    public:
        PhysicsBody(const Transform& transform,
                    const Collider& collider,
                    PhysicsProperties properties = PhysicsProperties{},
                    Vector3 linearFreedom = Vector3::One(),
                    Vector3 angularFreedom = Vector3::One());

        PhysicsBody(const PhysicsBody&) = delete;
        PhysicsBody(PhysicsBody&&) = default;
        PhysicsBody& operator=(const PhysicsBody&) = delete;
        PhysicsBody& operator=(PhysicsBody&&) = default;

        void SetProperties(const PhysicsProperties& properties);
        void SetLinearFreedom(const Vector3& linearFreedom);
        void SetAngularFreedom(const Vector3& angularFreedom);

        void SetMass(float mass) noexcept { m_properties.mass = mass == 0.0f ? 0.0f : 1.0f / mass; }
        void SetDrag(float drag) noexcept { m_properties.drag = drag; }
        void SetAngularDrag(float angularDrag) noexcept { m_properties.angularDrag = angularDrag; }
        void SetUseGravity(bool useGravity) noexcept { m_properties.useGravity = useGravity; }
        void SetIsKinematic(bool isKinematic) noexcept { m_properties.isKinematic = isKinematic; }

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
        auto GetLinearFreedom() const noexcept -> Vector3 { return ToVector3(GetLinearFreedomXM()); }
        auto GetAngularFreedom() const noexcept -> Vector3 { return ToVector3(GetAngularFreedomXM()); }
        auto GetLinearFreedomXM() const noexcept -> DirectX::FXMVECTOR { return m_linearFreedom; }
        auto GetAngularFreedomXM() const noexcept -> DirectX::FXMVECTOR { return m_angularFreedom; }
        auto GetVelocity() const noexcept -> DirectX::FXMVECTOR { return m_linearVelocity; }
        auto GetAngularVelocity() const noexcept -> DirectX::FXMVECTOR { return m_angularVelocity; }
        auto GetInverseMass() const noexcept -> float { return m_properties.mass; }
        auto GetDrag() const noexcept -> float { return m_properties.drag; }
        auto GetAngularDrag() const noexcept -> float { return m_properties.angularDrag; }
        auto GetInverseInertia() const noexcept -> DirectX::FXMMATRIX { return m_invInertiaWorld; }
        auto UseGravity() const noexcept -> bool { return m_properties.useGravity; }
        auto IsKinematic() const noexcept -> bool { return m_properties.isKinematic; }
        auto IsAwake() const noexcept -> bool { return m_awake; }

    private:
        DirectX::XMVECTOR m_linearVelocity;
        DirectX::XMVECTOR m_angularVelocity;
        DirectX::XMMATRIX m_invInertiaWorld;
        DirectX::XMVECTOR m_linearFreedom;
        DirectX::XMVECTOR m_angularFreedom;
        Vector3 m_invInertiaLocal;
        PhysicsProperties m_properties;
        uint8_t m_framesAtThreshold;
        bool m_awake;
};
} // namespace nc::physics
