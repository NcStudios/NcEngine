#include "MainMenuUI.h"
#include "imgui/imgui.h"
#include "Window.h"
#include "UIStyle.h"
#include "SceneManager.h"
#include "project/scenes/GameScene.h"
#include "nc/source/config/Version.h"
#include "nc/source/config/ConfigReader.h"

#include <fstream>
#include <functional>

namespace
{
    constexpr auto UI_FLAGS = ImGuiWindowFlags_NoCollapse |
                              ImGuiWindowFlags_NoTitleBar |
                              ImGuiWindowFlags_NoResize;

    const auto BUTTON_SIZE = ImVec2{200, 64};
    const auto SMALL_BUTTON_SIZE = ImVec2{96, 20};
    const auto LIST_BOX_SIZE = ImVec2{200, 64};
    const auto UI_SIZE = ImVec2{218, 450};

    const auto SERVER_PATH = std::string{"project/config/servers.ini"};

    void MapKeyValue(std::string key, std::string value, std::vector<project::ui::ServerSelectable>& out)
    {
        out.push_back(project::ui::ServerSelectable{std::move(key), std::move(value)});
    }

    void WriteServerRecords(const std::vector<project::ui::ServerSelectable>& records)
    {
        std::ofstream outFile;
        outFile.open(SERVER_PATH);
        for(auto& record : records)
        {
            outFile << record.name << '=' << record.ip << '\n';
        }
        outFile.close();
    }

    void UISpacing()
    {
        ImGui::Spacing(); ImGui::Spacing(); ImGui::Spacing();
    }

    void UISpacingWithSeparator()
    {
        ImGui::Spacing(); ImGui::Spacing(); ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing(); ImGui::Spacing(); ImGui::Spacing();
    }
}

namespace project::ui
{
    MainMenuUI::MainMenuUI()
        : m_config{nc::engine::Engine::GetConfig()},
          m_isHovered{false},
          m_servers{},
          m_editNameElement{false},
          m_addServerElement{false, std::bind(this->AddServer, this, std::placeholders::_1)}
    {
        m_ipBuffer[0] = '\0';
        SetImGuiStyle();
        nc::config::Read(SERVER_PATH, MapKeyValue, m_servers);
    }

    MainMenuUI::~MainMenuUI()
    {
        WriteServerRecords(m_servers);
    }

    void MainMenuUI::Draw()
    {
        auto dim = nc::Window::GetDimensions();
        ImGui::SetNextWindowPos( { (dim.X() - UI_SIZE.x) / 2, (dim.Y() - UI_SIZE.y) / 2 } );
        ImGui::SetNextWindowSize(UI_SIZE);

        if(ImGui::Begin("Caverna", nullptr, UI_FLAGS))
        {
            UISpacing();
            ImGui::Text("Caverna       version");
            ImGui::SameLine();
            ImGui::Text(NC_PROJECT_VERSION);

            UISpacingWithSeparator();

            ImGui::Text("Player Name:");
            ImGui::SameLine();
            ImGui::Text(m_config.user.userName.c_str());
            ImGui::Spacing();
            if(ImGui::Button("Edit", {40, 18}))
            {
                m_editNameElement.ToggleOpen();
            }

            UISpacingWithSeparator();

            ImGui::Text("Server IPs");
            ImGui::Spacing();
            ImGui::ListBoxHeader("", LIST_BOX_SIZE);
            for(auto& record : m_servers)
            {
                auto name = record.name + std::string{" - "} + record.ip;
                record.isSelected = ImGui::Selectable(name.c_str(), record.isSelected);
            }
            ImGui::ListBoxFooter();
            if(ImGui::Button("Add", SMALL_BUTTON_SIZE))
            {
                m_addServerElement.ToggleOpen();
            }
            ImGui::SameLine();
            if(ImGui::Button("Delete", SMALL_BUTTON_SIZE))
            {
                // not implemented
            }
            UISpacing();
            if(ImGui::Button("Connect to server", BUTTON_SIZE))
            { 
                // not implemented
            }

            UISpacingWithSeparator();

            if(ImGui::Button("Load Demo Scene", BUTTON_SIZE))
            {
                nc::scene::SceneManager::ChangeScene(std::make_unique<GameScene>());
            }
        }
        ImGui::End();

        m_editNameElement.Draw();
        m_addServerElement.Draw();
    }

    bool MainMenuUI::IsHovered()
    {
        return m_isHovered;
    }

    void MainMenuUI::AddServer(ServerSelectable server)
    {
        m_servers.push_back(server);
    }
}