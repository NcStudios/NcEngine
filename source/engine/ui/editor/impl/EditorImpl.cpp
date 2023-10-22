#include "ui/editor/Editor.h"
#include "EditorUI.h"
#include "input/Input.h"
#include "window/Window.h"

namespace
{
namespace hotkey
{
constexpr auto Editor = nc::input::KeyCode::Tilde;
} // namespace hotkey
} // anonymous namespace

namespace nc::ui::editor
{
class EditorImpl : public Editor
{
    public:
        void Draw(Registry* registry) override
        {
            if(input::KeyDown(hotkey::Editor))
                m_open = !m_open;

            if(!m_open)
                return;

            m_ui.Draw(registry);
        }

    private:
        EditorUI m_ui;
        bool m_open = false;
};

auto BuildEditor() -> std::unique_ptr<Editor>
{
    return std::make_unique<EditorImpl>();
}
} // namespace nc::ui::editor
