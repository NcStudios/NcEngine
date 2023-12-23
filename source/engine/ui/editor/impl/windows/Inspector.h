#pragma once

#include "ncengine/ecs/Ecs.h"
#include "ncengine/ecs/Entity.h"

namespace nc::ui::editor
{
class Inspector
{
    public:
        void Draw(ecs::Ecs world, Entity entity);
};
} // namespace nc::ui::editor
