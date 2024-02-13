#include "ui/editor/Editor.h"

namespace nc::ui::editor
{
struct EditorStub : public Editor
{
    EditorStub(const EditorContext& ctx)
        : Editor{ctx}
    {
    }

    void Draw(ecs::Ecs) override {}
};

auto BuildEditor(ecs::Ecs world, ModuleProvider modules, const EditorHotkeys& hotkeys) -> std::unique_ptr<Editor>
{
    return std::make_unique<EditorStub>(EditorContext
    {
        .world = world,
        .modules = modules,
        .selectedEntity = Entity::Null(),
        .openState = OpenState::ClosePersisted,
        .dimensions = ImVec2{},
        .objectBucket = Entity::Null(),
        .editorCamera = Entity::Null(),
        .hotkeys = hotkeys
    });
}
} // namespace nc::ui::editor
