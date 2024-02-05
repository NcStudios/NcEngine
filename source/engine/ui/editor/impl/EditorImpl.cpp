#include "ui/editor/Editor.h"
#include "EditorUI.h"
#include "ncengine/input/Input.h"
#include "ncengine/window/Window.h"

namespace
{
auto BuildContext(nc::ecs::Ecs world,
                  nc::ModuleProvider modules,
                  nc::ui::editor::EditorHotkeys hotkeys) -> nc::ui::editor::EditorContext
{
    // Parent Entity for all Editor objects
    const auto bucket = world.Emplace<nc::Entity>({
        .tag = "[Editor]",
        .flags = nc::Entity::Flags::Persistent |
                 nc::Entity::Flags::Internal |
                 nc::Entity::Flags::NoSerialize
    });

    // TODO put debug camera here

    return nc::ui::editor::EditorContext
    {
        .world = world,
        .modules = modules,
        .selectedEntity = nc::Entity::Null(),
        .objectBucket = bucket,
        .hotkeys = hotkeys,
        .openState = nc::ui::editor::OpenState::ClosePersisted
    };
}
} // anonymous namespace

namespace nc::ui::editor
{
class EditorImpl : public Editor
{
    public:
        explicit EditorImpl(ecs::Ecs world, ModuleProvider modules, const EditorHotkeys& hotkeys)
            : Editor{::BuildContext(world, modules, hotkeys)},
              m_ui{world, modules}
        {
        }

        void Draw(ecs::Ecs) override
        {
            m_ui.Draw(m_ctx);
        }

    private:
        EditorUI m_ui;
};

auto BuildEditor(ecs::Ecs world,
                 ModuleProvider modules,
                 const EditorHotkeys& hotkeys) -> std::unique_ptr<Editor>
{
    return std::make_unique<EditorImpl>(world, modules, hotkeys);
}
} // namespace nc::ui::editor
