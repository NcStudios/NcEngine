#ifdef NC_EDITOR_ENABLED
#include "Editor.h"
#include "EditorControls.h"
#include "Input.h"
#include "Window.h"

namespace
{
    constexpr auto MainWindowFlags = ImGuiWindowFlags_NoBackground |
                                     ImGuiWindowFlags_MenuBar |
                                     ImGuiWindowFlags_NoResize |
                                     ImGuiWindowFlags_NoScrollbar |
                                     ImGuiWindowFlags_NoMove;
    
    namespace hotkey
    {
        constexpr auto Editor = nc::input::KeyCode::Tilde;
        constexpr auto Utilities = nc::input::KeyCode::F1;
    }
}

namespace nc::ui::editor
{
    Editor::Editor()
        : m_openState_Editor{false},
          m_openState_UtilitiesPanel{true}
    {
    }

    void Editor::Frame(float* dt, Registry* registry)
    {
        if(input::GetKeyDown(hotkey::Editor))
            m_openState_Editor = !m_openState_Editor;

        if(!m_openState_Editor)
            return;
        
        if(input::GetKeyDown(hotkey::Utilities))
            m_openState_UtilitiesPanel = !m_openState_UtilitiesPanel;

        auto [width, height] = window::GetDimensions();
        ImGui::SetNextWindowPos({0,0});
        ImGui::SetNextWindowSize({width, height});
        if(ImGui::Begin("NcEngine Editor", nullptr, MainWindowFlags))
        {
            DrawMenu();
            controls::SceneGraphPanel(registry, height);
            if(m_openState_UtilitiesPanel)
                controls::UtilitiesPanel(dt, registry, width, height);
        }
        ImGui::End();
    }

    void Editor::DrawMenu()
    {
        if(ImGui::BeginMenuBar())
        {
            if(ImGui::BeginMenu("Window"))
            {
                ImGui::MenuItem("Utilities",     "F1", &m_openState_UtilitiesPanel);
                ImGui::EndMenu();
            }
            ImGui::EndMenuBar();
        }
    }
}
#endif