#include "MainMenuUI.h"
#include "UIStyle.h"
#include "Scene.h"
#include "scenes/DemoScene.h"
#include "scenes/GameScene.h"
#include "config/Version.h"
#include "config/ConfigReader.h"

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
    const auto UI_SIZE = nc::Vector2{218, 450};
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
    MainMenuUI::MainMenuUI(config::ProjectConfig projectConfig)
        : UIFixed(nc::ui::UIPosition::Center, UI_SIZE),
          m_projectConfig{ std::move(projectConfig) },
          m_isHovered{false},
          m_servers{},
          m_editNameElement{false, std::bind(this->EditName, this, std::placeholders::_1)},
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
        UIFixedElement::PositionElement();

        if(ImGui::Begin("Caverna", nullptr, UI_FLAGS))
        {
            UISpacing();
            ImGui::Text("Caverna       version");
            ImGui::SameLine();
            ImGui::Text(NC_PROJECT_VERSION);

            UISpacingWithSeparator();

            ImGui::Text("Player Name:");
            ImGui::SameLine();
            ImGui::Text(m_projectConfig.userName.c_str());
            ImGui::Spacing();
            if(ImGui::Button("Edit", {40, 18}))
            {
                m_editNameElement.ToggleOpen();
            }

            UISpacingWithSeparator();

            ImGui::Text("Server IPs");
            ImGui::Spacing();
            if(ImGui::ListBoxHeader("###ServerListBox", LIST_BOX_SIZE))
            {
                for(auto& record : m_servers)
                {
                    auto name = record.name + std::string{" - "} + record.ip;
                    if(ImGui::Selectable(name.c_str(), record.isSelected))
                    {
                        for(auto& record : m_servers)
                        {
                            record.isSelected = false;
                        }
                        record.isSelected = true;
                    }
                }
                ImGui::ListBoxFooter();
            }

            if(ImGui::Button("Add", SMALL_BUTTON_SIZE))
            {
                m_addServerElement.ToggleOpen();
            }
            ImGui::SameLine();
            if(ImGui::Button("Delete", SMALL_BUTTON_SIZE))
            {
                m_servers.erase(std::remove_if(m_servers.begin(), m_servers.end(), [](const ServerSelectable& server)
                {
                    return server.isSelected;
                }), m_servers.end());
            }
            UISpacing();
            if(ImGui::Button("Connect to server", BUTTON_SIZE))
            {
                auto selectedPos = std::find_if(m_servers.begin(), m_servers.end(), [](const ServerSelectable& server)
                {
                    return server.isSelected;
                });

                if(selectedPos != m_servers.end())
                {
                    nc::scene::Change(std::make_unique<GameScene>(selectedPos->ip));
                }
            }

            UISpacingWithSeparator();

            if(ImGui::Button("Load Demo Scene", BUTTON_SIZE))
            {
                nc::scene::Change(std::make_unique<DemoScene>());
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

    void MainMenuUI::EditName(std::string name)
    {
        m_projectConfig.userName = std::move(name);
        m_projectConfig.Save();
    }
}