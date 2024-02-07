#pragma once

#include "FactoryCommon.h"

namespace nc
{
void RegisterNetworkTypes(ecs::ComponentRegistry& registry, size_t maxEntities);
} // namespace nc
