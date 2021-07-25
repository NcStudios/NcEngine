#pragma once

#include "Ecs.h"
#include "../collision/Manifold.h"

namespace nc::physics
{
    /** Transform inertia tensors into updated world space. */
    void UpdateWorldInertiaTensors(registry_type* registry, std::span<PhysicsBody> bodies);
    
    /** Apply gravity to linear velocity of each body. */
    void ApplyGravity(std::span<PhysicsBody> bodies, float dt);
    
    /** Apply linear and angular velocities of each body to transforms. */
    void Integrate(registry_type* registry, std::span<PhysicsBody> bodies, float dt);
}