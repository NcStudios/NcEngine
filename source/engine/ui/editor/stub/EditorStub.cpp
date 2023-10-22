#include "ui/editor/Editor.h"

namespace nc::ui::editor
{
struct EditorStub : public Editor
{
    void Draw(Registry*) override {}
};

auto BuildEditor() -> std::unique_ptr<Editor>
{
    return std::make_unique<EditorStub>();
}
} // namespace nc::ui::editor
