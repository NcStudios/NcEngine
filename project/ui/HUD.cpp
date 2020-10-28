#include "HUD.h"
#include "imgui/imgui.h"
#include "graphics/d3dresource/GraphicsResource.h"
#include "LogUIElement.h"
#include "TurnPhaseUIElement.h"

namespace
{
    constexpr auto HUD_WINDOW_FLAGS = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize;
    constexpr unsigned RESOURCE_ICON_SIZE = 16;
}

namespace project::ui
{
    HUD::HUD()
        : m_logUIElement {true},
          m_turnPhaseUIElement {true},
          m_texture { std::make_unique<nc::graphics::d3dresource::Texture>("project/Textures/icon.bmp", 0) }
    {
    }

    HUD::~HUD()
    {
    }

    void HUD::Draw()
    {
        DrawHUD();
        m_logUIElement.Draw();
        m_turnPhaseUIElement.Draw();
    }

    void HUD::DrawHUD()
    {
        ImGui::SetNextWindowPos({0,0});
        ImGui::SetNextWindowSize({800, 60});

        if(ImGui::Begin("Hud", nullptr, HUD_WINDOW_FLAGS))
        {
            DrawMenu();
            DrawResources();

            ImGui::Spacing(); ImGui::SameLine(); ImGui::Spacing(); ImGui::SameLine();
            ImGui::Text("Turn: %d", 1); ImGui::SameLine();
            ImGui::Text("Phase: Harvest");
        }
        ImGui::End();
    }

    void HUD::DrawMenu()
    {
        if(ImGui::BeginMenuBar())
        {
            if(ImGui::BeginMenu("Log"))
            {
                if(ImGui::MenuItem("View"))
                {
                    m_logUIElement.ToggleOpen();
                }
                ImGui::EndMenu();
            }
            if(ImGui::BeginMenu("Helpers"))
            {
                if(ImGui::MenuItem("Turn Phases"))
                {
                    m_turnPhaseUIElement.ToggleOpen();
                }
                ImGui::EndMenu();
            }
            ImGui::EndMenuBar();
        }
    }

    void HUD::DrawResources()
    {
        DrawResource(m_texture.get(), 1, "Dwarves"); ImGui::SameLine();
        DrawResource(m_texture.get(), 4, "Food"); ImGui::SameLine();
        DrawResource(m_texture.get(), 1, "Grain"); ImGui::SameLine();
        DrawResource(m_texture.get(), 1, "Vegetable"); ImGui::SameLine();
        DrawResource(m_texture.get(), 1, "Dog"); ImGui::SameLine();
        DrawResource(m_texture.get(), 1, "Boar"); ImGui::SameLine();
        DrawResource(m_texture.get(), 1, "Sheep"); ImGui::SameLine();
        DrawResource(m_texture.get(), 1, "Cow"); ImGui::SameLine();
        DrawResource(m_texture.get(), 1, "Wood"); ImGui::SameLine();
        DrawResource(m_texture.get(), 1, "Ore"); ImGui::SameLine();
        DrawResource(m_texture.get(), 1, "Ruby"); ImGui::SameLine();
    }

    void HUD::DrawResource(nc::graphics::d3dresource::Texture* texture, unsigned count, const char* label)
    {
        ImGui::BeginGroup();
        ImGui::Image((void*)texture->GetShaderResourceView(), ImVec2(RESOURCE_ICON_SIZE, RESOURCE_ICON_SIZE));
        ImGui::SameLine();
        ImGui::Text("%d", count);
        ImGui::EndGroup();
        if(ImGui::IsItemHovered())
        {
            ImGui::SetTooltip(label);
        }
    }

} //end namespace project::ui