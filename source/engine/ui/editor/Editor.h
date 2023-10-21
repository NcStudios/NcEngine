#pragma once

#include <memory>

namespace nc
{
class Registry;

namespace ui::editor
{
class Editor
{
    public:
        virtual ~Editor() = default;
        virtual void Draw(Registry* registry) = 0;
};

auto BuildEditor() -> std::unique_ptr<Editor>;
} // namespace ui::editor
} // namespace nc
