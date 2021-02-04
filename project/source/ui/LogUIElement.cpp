#include "LogUIElement.h"
#include "project/source/log/GameLog.h"
#include "Input.h"

namespace project::ui
{
    LogUIElement::LogUIElement(bool startOpen, log::GameLog* gameLog)
        : nc::ui::UIElement(startOpen),
          m_gameLog{ gameLog }
    {
    }

    void LogUIElement::Draw()
    {
        auto width = ImGui::GetWindowSize().x;
        ImGui::SetNextItemWidth(width);
        ImGui::Text("Log");
        ImGui::Separator();
        for(auto& item : m_gameLog->GetItems())
        {
            ImGui::Text(item.c_str());
        }
    }
}