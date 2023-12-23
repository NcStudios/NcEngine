#pragma once

#include "ncengine/ecs/Ecs.h"
#include "ncengine/ecs/Entity.h"

namespace nc::ui::editor
{
auto EntityContextMenu(Entity entity, ecs::Ecs world) -> Entity;
void ModifyComponentList(Entity entity, ecs::Ecs world);
} // namespace nc::ui::editor
