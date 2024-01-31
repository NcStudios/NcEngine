#include "ui/editor/Editor.h"

namespace nc::ui::editor
{
struct EditorStub : public Editor
{
    void Draw(ecs::Ecs) override {}
};

auto BuildEditor(ecs::Ecs, ModuleProvider modules, const EditorHotkeys&) -> std::unique_ptr<Editor>
{
    return std::make_unique<EditorStub>();
}
} // namespace nc::ui::editor
