#pragma once

#include "FactoryCommon.h"

namespace nc
{
void RegisterAudioTypes(ecs::ComponentRegistry& registry, size_t maxEntities);
} // namespace nc
