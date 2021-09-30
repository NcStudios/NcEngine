#include "UtilitiesPanel.h"
#include "Window.h"

#include "imgui/imgui.h"

namespace
{
    const auto UtilitiesPanelHeight = 300;
}

namespace nc::editor
{
    UtilitiesPanel::UtilitiesPanel(Output* output)
        : m_dimensions{window::GetDimensions()},
          m_output{output}
    {
        window::RegisterOnResizeReceiver(this);
    }

    UtilitiesPanel::~UtilitiesPanel()
    {
        window::UnregisterOnResizeReceiver(this);
    }

    void UtilitiesPanel::Draw()
    {
        // yikes
        auto SceneGraphPanelWidth = 300;
        auto Padding = 2;


        const auto xPos = SceneGraphPanelWidth + 2.0f * Padding;
        auto panelWidth = m_dimensions.x - xPos;
        ImGui::SetNextWindowPos({xPos, m_dimensions.y - UtilitiesPanelHeight});

        if(ImGui::BeginChild("UtilitiesPanel", {panelWidth, UtilitiesPanelHeight}, true))
        {
            if(ImGui::BeginTabBar("UtilitiesLeftTabBar"))
            {
                if(ImGui::BeginTabItem("Output"))
                {
                    if(ImGui::Button("Clear"))
                        m_output->Clear();
                    
                    if(ImGui::BeginChild("", {0,0}, true))
                    {
                        m_output->Draw();
                    }
                    ImGui::EndChild();
                    ImGui::EndTabItem();
                }
                ImGui::EndTabBar();
            }
        }
        ImGui::EndChild();
    }

    void UtilitiesPanel::OnResize(Vector2 dimensions)
    {
        m_dimensions = dimensions;
    }
}