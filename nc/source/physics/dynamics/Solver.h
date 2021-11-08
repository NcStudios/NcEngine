#pragma once

#include "ecs/Registry.h"
#include "Constraint.h"
#include "Joint.h"

namespace nc::physics
{
    /** Create contact constraints for every contact point in each manifold. Also creates
     *  position constraints for each manifold if EnableDirectPositionCorrection == true. */
    void GenerateConstraints(Registry* registry, std::span<const Manifold> persistentManifolds, Constraints* out);
    
    /** Prepare joints for resolution. Applies worldpsace transformations and 
     *  precomputes bias/effective mass matrix. */
    void UpdateJoints(Registry* registry, std::span<Joint> joints, float dt);

    /** Resolve contact, position, and joint constraints. For contacts and joints, linear
     *  and angular velocities will be updated, but positions must be integrated separately.
     *  Position constraints will update transforms directly. */
    void ResolveConstraints(Constraints& constraints, std::span<Joint> joints, float dt);

    /** Store current frame impulses from constraints in each contact for warmstarting. */
    void CacheImpulses(std::span<const ContactConstraint> constraints, std::span<Manifold> manifolds);



    /** Resolve contact, position, and joint constraints. For contacts and joints, linear
     *  and angular velocities will be updated, but positions must be integrated separately.
     *  Position constraints will update transforms directly. */
    void ResolveConstraints(std::span<ContactConstraint*> contactConstraints, std::span<PositionConstraint*> positionConstraints, std::span<Joint*> joints, float dt);
} // namespace nc::physics