#pragma once

#include "Ecs.h"
#include "Constraint.h"
#include "physics/collision/Manifold.h"

namespace nc::physics
{
    /** Create ContactConstraints for every contact point in each manifold. If one of the objects does not have a
     *  PhysicsBody, create a single BasicContactConstraint for the entire manifold instead. */
    void GenerateConstraints(registry_type* registry, const std::vector<Manifold>& persistentManifolds, Constraints* constraints);
    
    /** Resolve all physics constraints. For ContactConstraints, PhysicsBody linear and angular velocities will be
     *  updated and must be integrated separately. For BasicContactConstraints, Transform positions will be directly
     *  modified. */
    void ResolveConstraints(Constraints* constraints, float dt);
} // namespace nc::physics