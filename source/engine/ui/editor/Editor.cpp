#ifdef NC_EDITOR_ENABLED
#include "Editor.h"
#include "input/Input.h"
#include "window/Window.h"

namespace
{
namespace hotkey
{
constexpr auto Editor = nc::input::KeyCode::Tilde;
}
} // anonymouse namespace

namespace nc::ui::editor
{
Editor::Editor()
    : m_ui{},
      m_open{false}
{
}

void Editor::Frame(float* dt, Registry* registry)
{
    if(input::KeyDown(hotkey::Editor))
        m_open = !m_open;

    if(!m_open)
        return;

    m_ui.Draw(registry, dt);
}
} // namespace nc::ui::editor
#endif
