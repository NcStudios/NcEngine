#pragma once

#include "FactoryCommon.h"

namespace nc
{
void RegisterPhysicsTypes(ecs::ComponentRegistry& registry, size_t maxEntities);
} // namespace nc
