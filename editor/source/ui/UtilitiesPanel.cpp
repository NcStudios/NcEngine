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
        auto panelWidth = m_dimensions.x - SceneGraphPanelWidth;

        ImGui::SetNextWindowPos(ImVec2{m_dimensions.x, m_dimensions.y}, ImGuiCond_Always, ImVec2{1.0f, 1.0f});
        ImGui::SetNextWindowSize(ImVec2{panelWidth, UtilitiesPanelHeight}, ImGuiCond_Once);

        if(ImGui::Begin("Utilities"))
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

        ImGui::End();
    }

    void UtilitiesPanel::OnResize(Vector2 dimensions)
    {
        m_dimensions = dimensions;
    }
}