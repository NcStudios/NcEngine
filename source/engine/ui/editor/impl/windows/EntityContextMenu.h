#pragma once

#include "ncengine/ecs/Entity.h"

namespace nc
{
class Registry;

namespace ui::editor
{
auto EntityContextMenu(Entity entity, Registry* registry) -> Entity;
void ModifyComponentList(Entity entity, Registry* registry);
} // namespace ui::editor
} // namespace nc
