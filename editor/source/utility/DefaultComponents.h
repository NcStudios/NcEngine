#pragma once

#include "Ecs.h"

namespace nc::editor
{
    /** Helper to create a mesh renderer using only default resources. */
    void AddDefaultMeshRenderer(registry_type* registry, Entity entity);
}