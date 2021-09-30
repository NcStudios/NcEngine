#pragma once

#include "Ecs.h"

namespace nc::editor
{
    /** Loads all resources that are part of the engine. */
    void LoadDefaultResources();

    /** Helper to create a mesh renderer using only default resources. */
    void AddDefaultMeshRenderer(registry_type* registry, Entity entity);
}