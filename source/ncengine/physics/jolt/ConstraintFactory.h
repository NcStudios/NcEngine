#pragma once

#include "ncengine/physics/Constraints.h"

namespace JPH
{
class Body;
class Constraint;
class PhysicsSystem;
} // namespace JPH

namespace nc::physics
{
class ConstraintFactory
{
    public:
        explicit ConstraintFactory(JPH::PhysicsSystem& physicsSystem)
            : m_physicsSystem{&physicsSystem}
        {
        }

        auto MakeConstraint(const ConstraintInfo& createInfo,
                            JPH::Body& first,
                            JPH::Body& second) -> JPH::Constraint*;

    private:
        JPH::PhysicsSystem* m_physicsSystem;
};
} // namespace nc::physics
