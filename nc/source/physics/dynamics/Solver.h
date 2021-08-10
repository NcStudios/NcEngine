#pragma once

#include "Ecs.h"
#include "Constraint.h"
#include "physics/collision/Manifold.h"

namespace nc::physics
{
    /** Create ContactConstraints for every contact point in each manifold. */
    auto GenerateConstraints(registry_type* registry, std::span<const Manifold> persistentManifolds) -> std::vector<ContactConstraint>;
    
    /** Resolve all physics constraints. Linear and angular velocities will be
     *  updated and must be integrated separately. */
    void ResolveConstraints(std::span<ContactConstraint> constraints, float dt);

    /** Store current frame impulses from constraints in each contact for warmstarting. */
    void CacheLagranges(std::span<Manifold> manifolds, std::span<ContactConstraint> constraints);
} // namespace nc::physics