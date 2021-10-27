#pragma once

#include "ecs/Registry.h"

namespace nc::physics
{
    /** Transform inertia tensors into updated world space. */
    void UpdateWorldInertiaTensors(Registry* registry);
    
    /** Apply gravity to linear velocity of each body. */
    void ApplyGravity(Registry* registry, float dt);
    
    /** Apply linear and angular velocities of each body to transforms. */
    void Integrate(Registry* registry, float dt);
}