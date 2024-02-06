#include "ui/editor/Editor.h"
#include "EditorCamera.h"
#include "EditorUI.h"
#include "ncengine/ecs/Logic.h"
#include "ncengine/ecs/InvokeFreeComponent.h"
#include "ncengine/input/Input.h"
#include "ncengine/window/Window.h"

namespace
{
auto BuildContext(nc::ecs::Ecs world,
                  nc::ModuleProvider modules,
                  nc::ui::editor::EditorHotkeys hotkeys) -> nc::ui::editor::EditorContext
{
    constexpr auto flags = nc::Entity::Flags::Persistent |
                           nc::Entity::Flags::Internal |
                           nc::Entity::Flags::NoSerialize;

    // Parent Entity for all Editor objects
    const auto bucket = world.Emplace<nc::Entity>(
    {
        .tag = "[Editor]",
        .flags = flags
    });

    // Editor camera
    const auto camera = world.Emplace<nc::Entity>(
    {
        .position = nc::Vector3{0.0f, 8.0f, -10.0f},
        .rotation = nc::Quaternion::FromEulerAngles(0.7f, 0.0f, 0.0f),
        .parent = bucket,
        .tag = "EditorCamera",
        .flags = flags
    });

    world.Emplace<nc::ui::editor::EditorCamera>(camera, modules, hotkeys.toggleEditorCamera);
    world.Emplace<nc::FrameLogic>(camera, nc::InvokeFreeComponent<nc::ui::editor::EditorCamera>{});

    return nc::ui::editor::EditorContext
    {
        .world = world,
        .modules = modules,
        .selectedEntity = nc::Entity::Null(),
        .openState = nc::ui::editor::OpenState::ClosePersisted,
        .objectBucket = bucket,
        .editorCamera = camera,
        .hotkeys = hotkeys
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
