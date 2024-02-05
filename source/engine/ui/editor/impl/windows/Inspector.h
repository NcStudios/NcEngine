#pragma once

#include "CreateEntityDialog.h"
#include "ncengine/ecs/Ecs.h"
#include "ncengine/ecs/Entity.h"

namespace nc::ui::editor
{
class Inspector
{
    public:
        void Draw(ecs::Ecs world, Entity entity, CreateEntityDialog& createEntityDialog);
};
} // namespace nc::ui::editor
