#pragma once

#include "ui/editor/Editor.h"

#include "ncengine/ecs/Ecs.h"
#include "ncengine/ecs/Entity.h"

namespace nc::ui::editor
{
class Inspector
{
    public:
        void Draw(EditorContext& ctx);
};
} // namespace nc::ui::editor
