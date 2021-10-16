#include "Output.h"

#include "imgui/imgui.h"

namespace
{
    const auto Red = ImVec4{0.9f, 0.1f, 0.1f, 1.0f};
    const auto Green = ImVec4{0.1f, 0.9f, 0.1f, 1.0f};
    const auto Grey = ImVec4{0.02f, 0.02f, 0.02f, 1.0f};
}

namespace nc::editor
{
    Output* Output::m_instance = nullptr;

    Output::Output()
    {
        Output::m_instance = this;
    }

    Output::~Output() noexcept
    {
        Output::m_instance = nullptr;
    }

    void Output::SetItemCount(unsigned count)
    {
        m_itemCount = count;
        while(m_items.size() > m_itemCount)
            m_items.pop_front();
    }
    
    void Output::Clear()
    {
        m_items.clear();
    }

    void Output::Log(std::string item, std::string description, bool isError)
    {
        auto& items = Output::m_instance->m_items;
        auto itemCount = Output::m_instance->m_itemCount;
        items.emplace_back(std::move(item), description, isError);
        if(items.size() > itemCount)
            items.pop_front();
    }

    void Output::LogError(std::string item, std::string description)
    {
        Log(std::move(item), std::move(description), true);
    }

    void Output::Draw()
    {
        ImGui::PushStyleColor(ImGuiCol_Button, Grey);
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, Grey);
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, Grey);

        for(const auto& item : m_items)
        {
            if(item.isError)
            {
                ImGui::PushStyleColor(ImGuiCol_Text, Red);
                ImGui::Button("!", {18, 18});
                ImGui::PopStyleColor(1);
            }
            else
            {
                ImGui::PushStyleColor(ImGuiCol_Text, Green);
                ImGui::Button(">", {18, 18});
                ImGui::PopStyleColor(1);
            }

            ImGui::SameLine();
            ImGui::Text(item.text.c_str());

            if(!item.description.empty())
            {
                ImGui::Text("    "); ImGui::SameLine();
                ImGui::Text(item.description.c_str());
            }

            ImGui::Separator();
        }

        ImGui::PopStyleColor(3);
    }
}