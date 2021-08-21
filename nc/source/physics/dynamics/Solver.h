#pragma once

#include "Ecs.h"
#include "Constraint.h"
#include "Joint.h"
#include "physics/collision/Manifold.h"

namespace nc::physics
{
    /** Create contact constraints for every contact point in each manifold. Also creates
     *  position constraints for each manifold if EnableDirectPositionCorrection == true. */
    auto GenerateConstraints(registry_type* registry, std::span<const Manifold> persistentManifolds) -> Constraints;
    
    /** Prepare joints for resolution. Applies worldpsace transformations and 
     *  precomputes bias/effective mass matrix. */
    void UpdateJoints(registry_type* registry, std::span<Joint> joints, float dt);

    /** Resolve contact, position, and joint constraints. For contacts and joints, linear
     *  and angular velocities will be updated, but positions must be integrated separately.
     *  Position constraints will update transforms directly. */
    void ResolveConstraints(Constraints& constraints, std::span<Joint> joints, float dt);

    /** Store current frame impulses from constraints in each contact for warmstarting. */
    void CacheLagranges(std::span<Manifold> manifolds, std::span<ContactConstraint> constraints);
} // namespace nc::physics