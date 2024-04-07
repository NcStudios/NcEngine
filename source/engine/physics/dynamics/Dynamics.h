#pragma once

#include "ecs/Registry.h"

namespace nc::physics
{
/** Apply gravity and update inertia tensors. */
void UpdatePhysicsBodies(Registry* registry, float dt);

/** Apply linear and angular velocities of each body to transforms. */
void Integrate(Registry* registry, float dt);
} // namespace nc::physics
