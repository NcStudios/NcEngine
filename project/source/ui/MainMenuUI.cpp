#include "MainMenuUI.h"
#include "imgui/imgui.h"
#include "Window.h"
#include "UIStyle.h"
#include "NcScene.h"
#include "project/scenes/GameScene.h"
#include "nc/source/config/Version.h"

namespace
{
    constexpr auto UI_FLAGS = ImGuiWindowFlags_NoCollapse |
                              ImGuiWindowFlags_NoTitleBar |
                              ImGuiWindowFlags_NoResize;

    const auto BUTTON_SIZE = ImVec2{200, 64};
    const auto SMALL_BUTTON_SIZE = ImVec2{96, 20};
    const auto LIST_BOX_SIZE = ImVec2{200, 64};
    const auto UI_SIZE = ImVec2{218, 450};
}

namespace project::ui
{
    MainMenuUI::MainMenuUI()
        : m_config{nc::config::NcGetConfigReference()},
          m_isHovered{false},
          m_editNameElement{false}
    {
        m_ipBuffer[0] = '\0';
        SetImGuiStyle();
    }

    MainMenuUI::~MainMenuUI()
    {
    }

    void MainMenuUI::Draw()
    {
        auto dim = nc::Window::GetDimensions();
        ImGui::SetNextWindowPos( { (dim.X() - UI_SIZE.x) / 2, (dim.Y() - UI_SIZE.y) / 2 } );
        ImGui::SetNextWindowSize(UI_SIZE);

        if(ImGui::Begin("Caverna", nullptr, UI_FLAGS))
        {
            ImGui::Spacing(); ImGui::Spacing(); ImGui::Spacing();

            ImGui::Text("Caverna       version");
            ImGui::SameLine();
            ImGui::Text(NC_PROJECT_VERSION);

            ImGui::Spacing(); ImGui::Spacing(); ImGui::Spacing();
            ImGui::Separator();
            ImGui::Spacing(); ImGui::Spacing(); ImGui::Spacing();

            ImGui::Text("Player Name:");
            ImGui::SameLine();
            ImGui::Text(m_config.user.userName.c_str());
            ImGui::Spacing();
            if(ImGui::Button("Edit", {40, 18}))
            {
                m_editNameElement.ToggleOpen();
            }

            ImGui::Spacing(); ImGui::Spacing(); ImGui::Spacing();
            ImGui::Separator();
            ImGui::Spacing(); ImGui::Spacing(); ImGui::Spacing();

            ImGui::Text("Server IPs");
            ImGui::Spacing();
            ImGui::ListBoxHeader("", LIST_BOX_SIZE);
            ImGui::ListBoxFooter();

            ImGui::Button("Add", SMALL_BUTTON_SIZE);
            ImGui::SameLine();
            ImGui::Button("Delete", SMALL_BUTTON_SIZE);

            ImGui::Spacing(); ImGui::Spacing(); ImGui::Spacing();

            if(ImGui::Button("Connect to server", BUTTON_SIZE))
            {
            }

            ImGui::Spacing(); ImGui::Spacing(); ImGui::Spacing();
            ImGui::Separator();
            ImGui::Spacing(); ImGui::Spacing(); ImGui::Spacing();

            if(ImGui::Button("Load Demo Scene", BUTTON_SIZE))
            {
                nc::scene::NcChangeScene(std::make_unique<GameScene>());
            }
        }
        ImGui::End();

        m_editNameElement.Draw();
    }

    bool MainMenuUI::IsHovered()
    {
        return m_isHovered;
    }
}