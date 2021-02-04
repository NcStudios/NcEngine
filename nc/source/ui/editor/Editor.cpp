#ifdef NC_EDITOR_ENABLED
#include "Editor.h"
#include "EditorControls.h"
#include "graphics/Graphics.h"
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
    Editor::Editor(graphics::Graphics * graphics)
        : m_graphics{graphics},
          m_openState_Editor{false},
          m_openState_UtilitiesPanel{true}
    {
    }

    void Editor::Frame(float* dt, ecs::EntityMap& activeEntities)
    {
        if(input::GetKeyDown(hotkey::Editor))
            m_openState_Editor = !m_openState_Editor;

        if(!m_openState_Editor)
            return;
        
        if(input::GetKeyDown(hotkey::Utilities))
            m_openState_UtilitiesPanel = !m_openState_UtilitiesPanel;

        if(ImGui::Begin("NcEngine Editor", nullptr, MainWindowFlags))
        {
            DrawMenu();
            auto [width, height] = window::GetDimensions();
            controls::SceneGraphPanel(activeEntities, height);
            if(m_openState_UtilitiesPanel)
                controls::UtilitiesPanel(dt, m_graphics->GetDrawCallCount(), width, height);
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