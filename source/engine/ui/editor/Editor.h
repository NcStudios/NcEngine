#pragma once

#include "ncengine/ecs/Ecs.h"

#include <memory>

namespace nc::ui::editor
{
class Editor
{
    public:
        virtual ~Editor() = default;
        virtual void Draw(ecs::Ecs world) = 0;
};

auto BuildEditor() -> std::unique_ptr<Editor>;
} // namespace nc::ui::editor
