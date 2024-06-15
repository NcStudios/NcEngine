#pragma once

#include "ncengine/ecs/Ecs.h"
#include "ncengine/input/Input.h"
#include "ncengine/module/ModuleProvider.h"
#include "ncengine/ui/ImGuiUtility.h"

namespace nc
{
struct SystemEvents;
}

namespace nc::ui::editor
{
struct EditorHotkeys
{
    input::KeyCode toggleEditor = input::KeyCode::Tilde;
    input::KeyCode openNewSceneDialog = input::KeyCode::F1;
    input::KeyCode openSaveSceneDialog = input::KeyCode::F2;
    input::KeyCode openLoadSceneDialog = input::KeyCode::F3;
    input::KeyCode toggleEditorCamera = input::KeyCode::F5;
};

enum class OpenState
{
    Closed,
    ClosePersisted,
    Opened,
    OpenPersisted
};

struct EditorContext
{
    // General game state
    ecs::Ecs world;
    ModuleProvider modules;
    SystemEvents* events = nullptr;

    // Mutable state
    Entity selectedEntity = Entity::Null();
    OpenState openState = OpenState::Closed;
    ImVec2 dimensions = ImVec2{};

    // Immutable state
    const Entity objectBucket = Entity::Null();
    const Entity editorCamera = Entity::Null();
    const EditorHotkeys hotkeys = EditorHotkeys{};
};
} // namespace nc::ui::editor
