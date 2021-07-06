#pragma once

#include "Ecs.h"
#include "Constraint.h"
#include "physics/collision/Manifold.h"

namespace nc::physics
{
    void GenerateConstraints(registry_type* registry, const std::vector<Manifold>& persistentManifolds, Constraints* constraints);
    void ResolveConstraint(const BasicContactConstraint& constraint);
    void ResolveConstraint(ContactConstraint& constraint, float dt);
} // namespace nc::physics