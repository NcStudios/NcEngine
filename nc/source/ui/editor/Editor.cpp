#ifdef NC_EDITOR_ENABLED
#include "Editor.h"
#include "EditorControls.h"
#include "graphics/Graphics.h"
#include "input/Input.h"
#include "Window.h"

namespace
{
    const auto TitleBarHeight = 40.0f;
    const auto MainWindowFlags = ImGuiWindowFlags_NoBackground |
                                 ImGuiWindowFlags_MenuBar |
                                 ImGuiWindowFlags_NoResize |
                                 ImGuiWindowFlags_NoScrollbar |
                                 ImGuiWindowFlags_NoMove;
}

namespace nc::ui::editor
{
    Editor::Editor(graphics::Graphics * graphics)
        : m_graphics{graphics},
          m_isGuiActive{false},
          m_openState_UtilitiesPanel{true},
          m_openState_GraphicsResources{false}
    {
    }

    void Editor::Frame(float* dt, ecs::EntityMap& activeEntities)
    {
        if(input::GetKeyUp(input::KeyCode::Tilde))
            ToggleGui();
        if(!m_isGuiActive)
            return;

        auto [width, height] = Window::GetDimensions();
        if(ImGui::Begin("NcEngine Editor", nullptr, MainWindowFlags))
        {
            DrawMenu();
            ImGui::SetNextWindowPos({controls::Padding, TitleBarHeight});
            controls::SceneGraphPanel(activeEntities);

            if(m_openState_GraphicsResources)
            {
                ImGui::SameLine();
                controls::GraphicsResourcePanel();
            }

            if(m_openState_UtilitiesPanel)
            {
                ImGui::SameLine(width - controls::PanelSize.x - controls::Padding);
                controls::UtilitiesPanel(dt, m_graphics->GetDrawCallCount());
            }
        }
        ImGui::End();
    }

    void Editor::ToggleGui() noexcept { m_isGuiActive = !m_isGuiActive; }
    void Editor::EnableGui() noexcept { m_isGuiActive = true; }
    void Editor::DisableGui() noexcept { m_isGuiActive = false; }
    bool Editor::IsGuiActive() const noexcept { return m_isGuiActive; }

    void Editor::DrawMenu()
    {
        if(ImGui::BeginMenuBar())
        {
            if(ImGui::BeginMenu("Window"))
            {
                ImGui::MenuItem("Utilities",     "F...", &m_openState_UtilitiesPanel);
                ImGui::MenuItem("Graphics Resources", "F...", &m_openState_GraphicsResources);
                ImGui::EndMenu();
            }
            ImGui::EndMenuBar();
        }
    }
}
#endif