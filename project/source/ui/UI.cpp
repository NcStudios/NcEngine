#include "UI.h"
#include "imgui/imgui.h"
#include "graphics/d3dresource/GraphicsResource.h"
#include "LogUIElement.h"
#include "TurnPhaseUIElement.h"
#include "project/source/log/GameLog.h"
#include "Window.h"

namespace
{
    constexpr auto HUD_WINDOW_FLAGS = ImGuiWindowFlags_MenuBar |
                                      ImGuiWindowFlags_NoCollapse |
                                      ImGuiWindowFlags_NoTitleBar |
                                      ImGuiWindowFlags_NoResize;
    constexpr auto HUD_HEIGHT = 200u;
    const auto RESOURCE_ICON_SIZE = ImVec2{16, 16};
    const auto PLAYER_PANEL_SIZE = ImVec2{220, 100};
    const auto LOG_PANEL_SIZE = ImVec2{400, 150};
}

namespace project::ui
{
    UI::UI(log::GameLog* gameLog)
        : m_config { ::nc::config::NcGetConfigReference() },
          m_logUIElement {true, gameLog},
          m_turnPhaseUIElement {false},
          m_editNameUIElement {false},
          m_soundboardUIElement {false},
          m_texture { std::make_unique<nc::graphics::d3dresource::Texture>("project/Textures/icon.bmp", 0) },
          m_isHovered { false }
    {
    }

    UI::~UI()
    {
    }

    void UI::Draw()
    {
        DrawHUD();
        m_turnPhaseUIElement.Draw();
        m_editNameUIElement.Draw();
        m_soundboardUIElement.Draw();
        m_isHovered = ImGui::IsAnyWindowHovered();
    }

    bool UI::IsHovered()
    {
        return m_isHovered;
    }

    void UI::DrawHUD()
    {
        auto dim = nc::Window::GetDimensions();
        ImGui::SetNextWindowPos({0, (float)dim.Y() - HUD_HEIGHT});
        ImGui::SetNextWindowSize({(float)dim.X(), HUD_HEIGHT});

        if(ImGui::Begin("Hud", nullptr, HUD_WINDOW_FLAGS))
        {
            DrawMenu();

            if(ImGui::BeginChild("PlayerPanel", PLAYER_PANEL_SIZE, true))
            {
                DrawTurnHeader();
                ImGui::Spacing();
                DrawResources();
            }
            ImGui::EndChild();

            ImGui::SameLine();

            if(ImGui::BeginChild("LogPanel", LOG_PANEL_SIZE, true))
            {
                m_logUIElement.Draw();
            }
            ImGui::EndChild();
        }
        ImGui::End();
    }

    void UI::DrawMenu()
    {
        if(ImGui::BeginMenuBar())
        {
            if(ImGui::BeginMenu("Player"))
            {
                if(ImGui::MenuItem("Edit Name"))
                {
                    m_editNameUIElement.ToggleOpen();
                }
                ImGui::EndMenu();
            }
            if(ImGui::BeginMenu("Windows"))
            {
                if(ImGui::MenuItem("Soundboard"))
                {
                    m_soundboardUIElement.ToggleOpen();
                }
                if(ImGui::MenuItem("Turn Phases"))
                {
                    m_turnPhaseUIElement.ToggleOpen();
                }
                ImGui::EndMenu();
            }
            if(ImGui::BeginMenu("Log"))
            {
                ImGui::EndMenu();
            }
            ImGui::EndMenuBar();
        }
    }

    void UI::DrawTurnHeader()
    {
        ImGui::Text(m_config.user.userName.c_str()); ImGui::SameLine();
        ImGui::Text("Turn: %d", 1); ImGui::SameLine();
        ImGui::Text("Phase: Harvest");
    }

    void UI::DrawResources()
    {
        DrawResource(m_texture.get(), 1, "Dwarves"); ImGui::SameLine();
        DrawResource(m_texture.get(), 4, "Food"); ImGui::SameLine();
        DrawResource(m_texture.get(), 1, "Ruby");
        DrawResource(m_texture.get(), 1, "Grain"); ImGui::SameLine();
        DrawResource(m_texture.get(), 1, "Vegetable"); ImGui::SameLine();
        DrawResource(m_texture.get(), 1, "Wood"); ImGui::SameLine();
        DrawResource(m_texture.get(), 1, "Ore");
        DrawResource(m_texture.get(), 1, "Dog"); ImGui::SameLine();
        DrawResource(m_texture.get(), 1, "Boar"); ImGui::SameLine();
        DrawResource(m_texture.get(), 1, "Sheep"); ImGui::SameLine();
        DrawResource(m_texture.get(), 1, "Cow"); ImGui::SameLine();
    }

    void UI::DrawResource(nc::graphics::d3dresource::Texture* texture, unsigned count, const char* label)
    {
        ImGui::BeginGroup();
        ImGui::Image((void*)texture->GetShaderResourceView(), RESOURCE_ICON_SIZE);
        ImGui::SameLine();
        ImGui::Text("%d", count);
        ImGui::EndGroup();
        if(ImGui::IsItemHovered())
        {
            ImGui::SetTooltip(label);
        }
    }

} //end namespace project::ui