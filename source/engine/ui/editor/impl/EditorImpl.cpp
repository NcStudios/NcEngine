#include "ui/Editor.h"
#include "EditorCamera.h"
#include "EditorUI.h"
#include "ncengine/ecs/Logic.h"
#include "ncengine/ecs/InvokeFreeComponent.h"
#include "ncengine/input/Input.h"
#include "ncengine/window/Window.h"

namespace
{
auto BuildEditorObjects(nc::ecs::Ecs world, nc::ModuleProvider modules, nc::input::KeyCode cameraHotkey) -> std::pair<nc::Entity, nc::Entity>
{
    // Parent Entity for all Editor objects
    const auto bucket = world.Emplace<nc::Entity>({
        .tag = "[Editor]",
        .flags = nc::ui::editor::EditorObjectFlags
    });

    // Editor camera
    const auto camera = world.Emplace<nc::Entity>({
        .position = nc::Vector3{0.0f, 8.0f, -10.0f},
        .rotation = nc::Quaternion::FromEulerAngles(0.7f, 0.0f, 0.0f),
        .parent = bucket,
        .tag = "EditorCamera",
        .flags = nc::ui::editor::EditorObjectFlags
    });

    world.Emplace<nc::ui::editor::EditorCamera>(camera, modules, cameraHotkey);
    world.Emplace<nc::FrameLogic>(camera, nc::InvokeFreeComponent<nc::ui::editor::EditorCamera>{});
    return std::pair{bucket, camera};
}

auto BuildContext(nc::ecs::Ecs world,
                  nc::ModuleProvider modules,
                  nc::SystemEvents& events,
                  nc::ui::editor::EditorHotkeys hotkeys) -> nc::ui::editor::EditorContext
{
    const auto [bucket, camera] = ::BuildEditorObjects(world, modules, hotkeys.toggleEditorCamera);
    return nc::ui::editor::EditorContext{
        .world = world,
        .modules = modules,
        .events = &events,
        .selectedEntity = nc::Entity::Null(),
        .openState = nc::ui::editor::OpenState::ClosePersisted,
        .dimensions = ImVec2{},
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
        explicit EditorImpl(ecs::Ecs world, ModuleProvider modules, SystemEvents& events, const EditorHotkeys& hotkeys)
            : Editor{::BuildContext(world, modules, events, hotkeys)},
              m_ui{m_ctx}
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
                 SystemEvents& events,
                 const EditorHotkeys& hotkeys) -> std::unique_ptr<Editor>
{
    return std::make_unique<EditorImpl>(world, modules, events, hotkeys);
}
} // namespace nc::ui::editor
