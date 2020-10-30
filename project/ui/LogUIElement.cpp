#include "LogUIElement.h"

namespace project::ui
{
    LogUIElement::LogUIElement(bool startOpen)
        : UIElement(startOpen)
    {
    }

    void LogUIElement::Draw()
    {
        if(!isOpen) return;

        if(ImGui::Begin("Log", &(this->isOpen), ImGuiWindowFlags_NoCollapse))
        {
            ImGui::Text("Entry");
            ImGui::Text("Entry");
            ImGui::Text("Entry");
            ImGui::Text("Entry");
            ImGui::Text("Entry");
            ImGui::Text("Entry");
            ImGui::Text("Entry");
        }
        ImGui::End();
    }
}