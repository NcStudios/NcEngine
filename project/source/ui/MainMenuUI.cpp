#include "MainMenuUI.h"
#include "imgui/imgui.h"
#include "Window.h"
#include "UIStyle.h"
#include "NcScene.h"
#include "project/scenes/GameScene.h"
#include "nc/source/config/Version.h"

#include <fstream>

namespace
{
    constexpr auto UI_FLAGS = ImGuiWindowFlags_NoCollapse |
                              ImGuiWindowFlags_NoTitleBar |
                              ImGuiWindowFlags_NoResize;

    const auto BUTTON_SIZE = ImVec2{200, 64};
    const auto SMALL_BUTTON_SIZE = ImVec2{96, 20};
    const auto LIST_BOX_SIZE = ImVec2{200, 64};
    const auto UI_SIZE = ImVec2{218, 450};

    const auto INI_SKIP_CHARS = std::string{";#["};
    const auto INI_KEY_VALUE_DELIM = '=';

    bool ParseLine(const std::string& line, std::string& key, std::string& value)
    {
        if (INI_SKIP_CHARS.find(line[0]) != INI_SKIP_CHARS.npos)
        {
            return false;
        }

        auto delimIndex = line.find(INI_KEY_VALUE_DELIM);
        if (delimIndex == line.npos)
        {
            return false;
        }

        key = line.substr(0, delimIndex);
        value = line.substr(delimIndex + 1);
        return true;
    }

    std::vector<project::ui::ServerSelectable> ReadServerRecords()
    {
        std::ifstream inFile;
        inFile.open("project/config/servers.ini");
        if(!inFile.is_open())
        {
            throw std::runtime_error("Could not open server record file");
        }

        std::vector<project::ui::ServerSelectable> out;
        std::string line{}, name{}, ip{};
        while(!inFile.eof())
        {
            if(inFile.fail())
            {
                throw std::runtime_error("Loading config - ifstream failure");
            }

            std::getline(inFile, line, '\n');
            if (ParseLine(line, name, ip))
            {
                out.push_back( {name, ip, false} );
            }
        }

        return out;
    }

    void WriteServerRecords(const std::vector<project::ui::ServerSelectable>& records)
    {
        std::ofstream outFile;
        outFile.open("project/config/servers.ini");
        for(auto& record : records)
        {
            outFile << record.name << '=' << record.ip << '\n';
        }
        outFile.close();
    }
}

namespace project::ui
{
    MainMenuUI::MainMenuUI()
        : m_config{nc::config::NcGetConfigReference()},
          m_isHovered{false},
          m_servers{ReadServerRecords()},
          m_editNameElement{false},
          m_addServerElement{false, m_servers}
    {
        m_ipBuffer[0] = '\0';
        SetImGuiStyle();
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
        m_addServerElement.Draw();
    }

    bool MainMenuUI::IsHovered()
    {
        return m_isHovered;
    }
}