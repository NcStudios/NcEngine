/**
 * @file EditorContext.h
 * @copyright Jaremie Romer and McCallister Romer 2024
 */
#pragma once

#include "ncengine/ecs/Ecs.h"
#include "ncengine/input/Input.h"
#include "ncengine/module/ModuleProvider.h"
#include "ncengine/ui/ImGuiUtility.h"

namespace nc
{
struct SystemEvents;

namespace ui::editor
{
/** @brief Hotkeys for the editor */
struct EditorHotkeys
{
    input::KeyCode toggleEditor = input::KeyCode::Tilde;
    input::KeyCode openNewSceneDialog = input::KeyCode::F1;
    input::KeyCode openSaveSceneDialog = input::KeyCode::F2;
    input::KeyCode openLoadSceneDialog = input::KeyCode::F3;
    input::KeyCode toggleEditorCamera = input::KeyCode::F5;
};

/** @brief Signals editor open and close events */
enum class OpenState
{
    Closed,         // Editor was closed this frame
    ClosePersisted, // Editor remains closed
    Opened,         // Editor was opened this frame
    OpenPersisted   // Editor remains open
};

/** @brief State for the editor */
struct EditorContext
{
    // General game state
    ecs::Ecs world;
    ModuleProvider modules;
    SystemEvents* events = nullptr;

    // Mutable state (for internal use)
    Entity selectedEntity = Entity::Null();
    OpenState openState = OpenState::Closed;
    ImVec2 dimensions = ImVec2{};

    // Immutable state
    const Entity objectBucket = Entity::Null();
    const Entity editorCamera = Entity::Null();
    const EditorHotkeys hotkeys = EditorHotkeys{};
};
} // namespace ui::editor
} // namespace nc
