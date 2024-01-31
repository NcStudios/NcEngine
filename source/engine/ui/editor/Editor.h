#pragma once

#include "ncengine/asset/NcAsset.h"
#include "ncengine/ecs/Ecs.h"
#include "ncengine/input/Input.h"

#include <memory>

namespace nc
{
class Scene;

namespace ui::editor
{
struct EditorHotkeys
{
    input::KeyCode toggleEditor = input::KeyCode::Tilde;
    input::KeyCode openNewSceneDialog = input::KeyCode::F1;
    input::KeyCode openSaveSceneDialog = input::KeyCode::F2;
    input::KeyCode openLoadSceneDialog = input::KeyCode::F3;
};

class Editor
{
    public:
        explicit Editor(const EditorHotkeys& hotkeys) noexcept
            : m_hotkeys{hotkeys} {}

        virtual ~Editor() = default;
        virtual void Draw(ecs::Ecs world, asset::NcAsset& assetModule) = 0;

        auto GetHotkeys() const noexcept -> const EditorHotkeys&
        {
            return m_hotkeys;
        }

    private:
        EditorHotkeys m_hotkeys;
};

auto BuildEditor(ecs::Ecs world,
                 std::function<void(std::unique_ptr<Scene>)> changeScene,
                 const EditorHotkeys& hotkeys = EditorHotkeys{}) -> std::unique_ptr<Editor>;
} // namespace ui::editor
} // namespace nc
