#pragma once
#ifdef NC_EDITOR_ENABLED
#include "imgui/imgui.h"

namespace nc::ui::editor
{
    const float defaultItemWidth = 50.0f;

    namespace color
    {
        const auto White = ImVec4{1.0f, 1.0f, 1.0f, 1.0f};
        const auto Black = ImVec4{0.0f, 0.0f, 0.0f, 1.0f};
        const auto Clear = ImVec4{0.0f, 0.0f, 0.0f, 0.0f};
        const auto Red =   ImVec4{1.0f, 0.2f, 0.1f, 1.0f};
        const auto Green = ImVec4{0.0f, 1.0f, 0.0f, 1.0f};
        const auto Blue =  ImVec4{0.0f, 0.4f, 1.0f, 1.0f};
    }

    inline bool floatWidget(const char* label, const char* id, float* item, float dragSpeed = 0.1f, float min = 0.1f, float max = 10.0f, const char* fmt = "%.1f")
    {
        ImGui::PushID(id);
        ImGui::Text(label);
        ImGui::SameLine();
        ImGui::SetNextItemWidth(defaultItemWidth);
        auto result = ImGui::DragFloat("", item, dragSpeed, min, max, fmt);
        ImGui::PopID();
        return result;
    };

    inline void textBlockWidget(const char* label, ImVec2 size, ImVec4 bgColor, ImVec4 textColor)
    {
        ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_Button, bgColor);
        ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_ButtonHovered, bgColor);
        ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_ButtonActive, bgColor);
        ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_Text, textColor);
        ImGui::Button(label, size);
        ImGui::PopStyleColor(4);
    }

    inline void xyzWidgetHeader(const char* frontPadding)
    {
        const ImVec2 buttonSize{defaultItemWidth, 0};

        ImGui::BeginGroup();
        ImGui::PushItemWidth(defaultItemWidth);
        ImGui::Indent();
            ImGui::Text(frontPadding); ImGui::SameLine();
            textBlockWidget("X##widgetHeader", buttonSize, color::Clear, color::Red);   ImGui::SameLine();
            textBlockWidget("Y##widgetHeader", buttonSize, color::Clear, color::Green); ImGui::SameLine();
            textBlockWidget("Z##widgetHeader", buttonSize, color::Clear, color::Blue);
        ImGui::PopItemWidth();
        ImGui::EndGroup();
    }

    inline bool xyzWidget(const char* groupLabel, const char* id, float* x, float* y, float* z, float min = -50.0f, float max = 50.0f)
    {
        ImGui::PushID(id);
        ImGui::PushItemWidth(defaultItemWidth);
        ImGui::BeginGroup();
            ImGui::Indent();
            ImGui::Text(groupLabel); ImGui::SameLine();
            auto xResult = floatWidget("", "x", x, 0.1f, min, max, "%.1f"); ImGui::SameLine();
            auto yResult = floatWidget("", "y", y, 0.1f, min, max, "%.1f"); ImGui::SameLine();
            auto zResult = floatWidget("", "z", z, 0.1f, min, max, "%.1f");
        ImGui::EndGroup();
        ImGui::PopItemWidth();
        ImGui::PopID();

        return xResult || yResult || zResult;
    };

    inline void columnHeaderWidget(const char* frontPadding, const char* label1, const char* label2, const char* label3 = nullptr)
    {
        const ImVec2 buttonSize{defaultItemWidth, 0};

        ImGui::BeginGroup();
        ImGui::PushItemWidth(defaultItemWidth);
        ImGui::Indent();
            ImGui::Text(frontPadding); ImGui::SameLine();
            textBlockWidget(label1, buttonSize, color::Clear, color::White); ImGui::SameLine();
            textBlockWidget(label2, buttonSize, color::Clear, color::White); ImGui::SameLine();
            if(label3)
                textBlockWidget(label3, buttonSize, color::Clear, color::White);
        ImGui::PopItemWidth();
        ImGui::EndGroup();
    }
}

#endif