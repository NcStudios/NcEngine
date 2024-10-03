#pragma once

#include "ncengine/ecs/Ecs.h"

namespace nc::sample
{
/** Frame Logic */
void WasdBasedMovement(Entity self, ecs::Ecs world, float dt);
void WasdBasedSimulatedBodyMovement(Entity self, ecs::Ecs world, float dt);
} // namespace nc::sample
