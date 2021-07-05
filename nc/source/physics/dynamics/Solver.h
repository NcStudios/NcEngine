#pragma once

#include "Ecs.h"
#include "Constraint.h"
#include "physics/collision/Manifold.h"

namespace nc::physics
{
    void GenerateConstraints(const std::vector<Manifold>& persistentManifolds, Constraints* constraints);
    void ResolveConstraint(const BasicContactConstraint& constraint);
    void ResolveConstraint(ContactConstraint& constraint, float dt);
    void ApplyGravity(float dt);
    void Integrate(float dt);
} // namespace nc::physics