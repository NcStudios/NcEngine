#pragma once

#include "Ecs.h"

namespace nc::physics
{
    /** Matrix representing a 12 element vector of velocity components.
     *  V = [Vax, Vay, Vaz,  Wax, Way, Waz,  Vbx, Vby, Vbz,  Wbx, Wby, Wbz] */
    class ConstraintMatrix
    {
        public:
            ConstraintMatrix() = default;
            ConstraintMatrix(const Vector3& vA, const Vector3& wA, const Vector3& vB, const Vector3& wB);
            ConstraintMatrix(DirectX::FXMVECTOR vA, DirectX::FXMVECTOR wA, DirectX::FXMVECTOR vB, DirectX::FXMVECTOR wB);

            static auto VelocityJacobian(DirectX::FXMVECTOR a, DirectX::FXMVECTOR b, DirectX::FXMVECTOR direction) -> ConstraintMatrix;

            auto vA() const -> DirectX::FXMVECTOR { return m_data.r[0]; }
            auto wA() const -> DirectX::FXMVECTOR { return m_data.r[1]; }
            auto vB() const -> DirectX::FXMVECTOR { return m_data.r[2]; }
            auto wB() const -> DirectX::FXMVECTOR { return m_data.r[3]; }

            auto vA() -> DirectX::XMVECTOR& { return m_data.r[0]; }
            auto wA() -> DirectX::XMVECTOR& { return m_data.r[1]; }
            auto vB() -> DirectX::XMVECTOR& { return m_data.r[2]; }
            auto wB() -> DirectX::XMVECTOR& { return m_data.r[3]; }

        private:
            DirectX::XMMATRIX m_data;
    };

    /** Resolve collisions involving a non-PhysicsBody.
     *  @todo What should happen in non-PhysicsBody vs. PhysicsBody?
     *    - move non-PhysicsBody / move both / apply force(how?)... */
    struct BasicContactConstraint
    {
        Transform *transformA, *transformB; // null for static entities
        Vector3 mtv;
    };

    /** Resolve PhysicsBody vs. PhysicsBody collisions. */
    struct ContactConstraint
    {
        Entity entityA, entityB;
        PhysicsBody* physBodyA, *physBodyB;
        ConstraintMatrix jNormal, jTangent, jBitangent;
        DirectX::XMMATRIX invInertiaA, invInertiaB;
        DirectX::XMVECTOR normal, rA, rB, effectiveMass;
        float penetrationDepth;
        float invMassA, invMassB;
        float totalLambda, totalMuTangent, totalMuBitangent;
    };

    /** Collection of constraints. */
    struct Constraints
    {
        std::vector<BasicContactConstraint> basic;
        std::vector<ContactConstraint> contact;
    };

    inline ConstraintMatrix::ConstraintMatrix(const Vector3& vA, const Vector3& wA, const Vector3& vB, const Vector3& wB)
        : m_data{DirectX::XMLoadVector3(&vA),
                 DirectX::XMLoadVector3(&wA),
                 DirectX::XMLoadVector3(&vB),
                 DirectX::XMLoadVector3(&wB)}
    {
    }

    inline ConstraintMatrix::ConstraintMatrix(DirectX::FXMVECTOR vA, DirectX::FXMVECTOR wA, DirectX::FXMVECTOR vB, DirectX::FXMVECTOR wB)
        : m_data{vA, wA, vB, wB}
    {
    }

    inline auto ConstraintMatrix::VelocityJacobian(DirectX::FXMVECTOR a, DirectX::FXMVECTOR b, DirectX::FXMVECTOR direction) -> ConstraintMatrix
    {
        return ConstraintMatrix
        {
            DirectX::XMVectorNegate(direction),
            DirectX::XMVectorNegate(DirectX::XMVector3Cross(a, direction)),
            direction,
            DirectX::XMVector3Cross(b, direction)
        };
    }
}