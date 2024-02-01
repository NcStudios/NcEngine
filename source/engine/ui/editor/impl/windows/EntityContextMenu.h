#pragma once

#include "CreateEntityWindow.h"
#include "ncengine/ecs/Ecs.h"
#include "ncengine/ecs/Entity.h"

namespace nc::ui::editor
{
auto EntityContextMenu(Entity entity, ecs::Ecs world, CreateEntityWindow& createEntityWindow) -> Entity;
void ModifyComponentList(Entity entity, ecs::Ecs world);
} // namespace nc::ui::editor
