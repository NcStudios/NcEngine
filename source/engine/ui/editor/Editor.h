#pragma once

#include "ncengine/ecs/Ecs.h"
#include "ncengine/input/Input.h"
#include "ncengine/module/ModuleProvider.h"

#include <memory>

namespace nc::ui::editor
{
struct EditorHotkeys
{
    input::KeyCode toggleEditor = input::KeyCode::Tilde;
    input::KeyCode openNewSceneDialog = input::KeyCode::F1;
    input::KeyCode openSaveSceneDialog = input::KeyCode::F2;
    input::KeyCode openLoadSceneDialog = input::KeyCode::F3;
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
    ecs::Ecs world;
    ModuleProvider modules;
    Entity selectedEntity;
    Entity objectBucket;
    EditorHotkeys hotkeys;
    OpenState openState;
};

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
