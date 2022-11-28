#pragma once

#include "ncengine/ecs/Registry.h"

namespace nc::sample
{
/** Frame Logic */
void WasdBasedMovement(Entity self, Registry* registry, float dt);

/** Fixed Logic */
void ForceBasedMovement(Entity self, Registry* registry);

/** Collision Logic */
void DestroyOnTriggerExit(Entity self, Entity hit, Registry* registry);
void LogOnCollisionEnter(Entity self, Entity hit, Registry* registry);
void LogOnCollisionExit(Entity self, Entity hit, Registry* registry);
void LogOnTriggerEnter(Entity self, Entity hit, Registry* registry);
void LogOnTriggerExit(Entity self, Entity hit, Registry* registry);
}
