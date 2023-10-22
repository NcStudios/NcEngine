#pragma once

#include "ncengine/ecs/Entity.h"

namespace nc
{
class Registry;

namespace ui::editor
{
class Inspector
{
    public:
        void Draw(Registry* registry, Entity entity);
};
} // namespace ui::editor
} // namespace nc
