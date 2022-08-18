#pragma once

#include "ecs/Registry.h"
#include "DirectXMath.h"

#include <vector>

namespace nc::physics
{
class PhysicsBody;

struct Joint
{
    Entity entityA;
    Entity entityB;
    PhysicsBody* bodyA;
    PhysicsBody* bodyB;
    DirectX::XMVECTOR anchorA;
    DirectX::XMVECTOR anchorB;
    DirectX::XMMATRIX m;
    DirectX::XMVECTOR rA, rB, bias, p;
    float biasFactor, softness;
};

class JointSystem
{
    public:
        JointSystem(Registry* registry);

        auto Joints() -> std::span<Joint> { return m_joints; }

        /** Prepare joints for resolution. Applies worldpsace transformations and
        *   precomputes bias/effective mass matrix. */
        void UpdateJoints(float dt);

        void AddJoint(Entity entityA, Entity entityB, const Vector3& anchorA, const Vector3& anchorB, float bias, float softness) noexcept;
        void RemoveJoint(Entity entityA, Entity entityB) noexcept;
        void RemoveAllJoints(Entity entity) noexcept;
        void Clear() noexcept;

    private:
        Registry* m_registry;
        std::vector<Joint> m_joints;

        void UpdateJoint(Joint& joint, float dt);
};
} // namespace nc::physics
