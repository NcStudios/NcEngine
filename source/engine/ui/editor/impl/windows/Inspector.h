#pragma once

#include "CreateEntityWindow.h"
#include "ncengine/ecs/Ecs.h"
#include "ncengine/ecs/Entity.h"

namespace nc::ui::editor
{
class Inspector
{
    public:
        void Draw(ecs::Ecs world, Entity entity, CreateEntityWindow& createEntityWindow);
};
} // namespace nc::ui::editor
