#include "ui/editor/Editor.h"
#include "EditorUI.h"
#include "input/Input.h"
#include "window/Window.h"

namespace nc::ui::editor
{
class EditorImpl : public Editor
{
    public:
        explicit EditorImpl(ecs::Ecs world, const EditorHotkeys& hotkeys)
            : Editor{hotkeys},
              m_ui{world}
        {
        }

        void Draw(ecs::Ecs world, asset::NcAsset& ncAsset) override
        {
            // if(input::KeyDown(GetHotkeys().toggleEditor))
            //     m_open = !m_open;

            // if(!m_open)
            //     return;

            m_ui.Draw(GetHotkeys(), world, ncAsset);
        }

    private:
        EditorUI m_ui;
        // bool m_open = false;
};

auto BuildEditor(ecs::Ecs world, const EditorHotkeys& hotkeys) -> std::unique_ptr<Editor>
{
    return std::make_unique<EditorImpl>(world, hotkeys);
}
} // namespace nc::ui::editor
