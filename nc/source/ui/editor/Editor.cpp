#ifdef NC_EDITOR_ENABLED
#include "Editor.h"
#include "EditorControls.h"
#include "graphics/Graphics.h"
#ifdef USE_VULKAN
#include "graphics/Graphics2.h"
#endif
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
    #ifdef USE_VULKAN
    Editor::Editor(graphics::Graphics2 * graphics, const ecs::Systems& systems)
        : m_graphics2{graphics},
          m_componentSystems{systems},
          m_openState_Editor{false},
          m_openState_UtilitiesPanel{true}
    {
    }
    #endif

    Editor::Editor(graphics::Graphics * graphics, const ecs::Systems& systems)
        : m_graphics{graphics},
          m_componentSystems{systems},
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

        auto [width, height] = window::GetDimensions();
        ImGui::SetNextWindowPos({0,0});
        ImGui::SetNextWindowSize({width, height});
        if(ImGui::Begin("NcEngine Editor", nullptr, MainWindowFlags))
        {
            DrawMenu();
            controls::SceneGraphPanel(activeEntities, height);
            if(m_openState_UtilitiesPanel)
                controls::UtilitiesPanel(dt, &m_componentSystems, m_graphics->GetDrawCallCount(), width, height);
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