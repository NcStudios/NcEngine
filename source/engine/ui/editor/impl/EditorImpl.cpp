#include "ui/editor/Editor.h"
#include "EditorUI.h"
#include "input/Input.h"
#include "window/Window.h"

namespace nc::ui::editor
{
class EditorImpl : public Editor
{
    public:
        explicit EditorImpl(ecs::Ecs world, std::function<void(std::unique_ptr<Scene>)> changeScene, const EditorHotkeys& hotkeys)
            : Editor{hotkeys},
              m_ui{world, std::move(changeScene)}
        {
        }

        void Draw(ecs::Ecs world, asset::NcAsset& ncAsset) override
        {
            m_ui.Draw(GetHotkeys(), world, ncAsset);
        }

    private:
        EditorUI m_ui;
};

auto BuildEditor(ecs::Ecs world,
                 std::function<void(std::unique_ptr<Scene>)> changeScene,
                 const EditorHotkeys& hotkeys) -> std::unique_ptr<Editor>
{
    return std::make_unique<EditorImpl>(world, std::move(changeScene), hotkeys);
}
} // namespace nc::ui::editor
