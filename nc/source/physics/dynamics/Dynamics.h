#pragma once

#include "Ecs.h"
#include "../collision/Manifold.h"

namespace nc::physics
{
    /** Transform inertia tensors into updated world space. */
    void UpdateWorldInertiaTensors(registry_type* registry);
    
    /** Apply gravity to linear velocity of each body. */
    void ApplyGravity(registry_type* registry, float dt);
    
    /** Apply linear and angular velocities of each body to transforms. */
    void Integrate(registry_type* registry, float dt);
}