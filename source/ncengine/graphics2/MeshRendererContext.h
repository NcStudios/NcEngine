#pragma once

#include "frontend/subsystem/MeshRendererCache.h"
#include "frontend/subsystem/MaterialPassCache.h"

namespace nc::graphics
{
// todo: this doesn't have to be a ctx object, just expose MeshRendererSubsystem
struct MeshRendererContext
{
    InstanceCache& instanceCache;
    MaterialPassCache& passCache;
};
} // namespace nc::graphics
