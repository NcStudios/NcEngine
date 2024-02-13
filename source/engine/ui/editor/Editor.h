#pragma once

#include "ncengine/ecs/Ecs.h"
#include "ncengine/input/Input.h"
#include "ncengine/module/ModuleProvider.h"
#include "ncengine/ui/ImGuiUtility.h"

#include <memory>

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

    // Mutable state
    Entity selectedEntity;
    OpenState openState;
    ImVec2 dimensions;

    // Immutable state
    const Entity objectBucket;
    const Entity editorCamera;
    const EditorHotkeys hotkeys;
};

// Flags to keep editor objects out of the way
constexpr auto EditorObjectFlags = nc::Entity::Flags::Persistent |
                                   nc::Entity::Flags::Internal |
                                   nc::Entity::Flags::NoSerialize;

class Editor
{
    public:
        explicit Editor(const EditorContext& ctx) noexcept
            : m_ctx{ctx} {}

        virtual ~Editor() = default;
        virtual void Draw(ecs::Ecs world) = 0;

        auto GetContext() const noexcept -> const EditorContext&
        {
            return m_ctx;
        }

    protected:
        EditorContext m_ctx;
};

auto BuildEditor(ecs::Ecs world,
                 ModuleProvider modules,
                 const EditorHotkeys& hotkeys = EditorHotkeys{}) -> std::unique_ptr<Editor>;
} // namespace nc::ui::editor
