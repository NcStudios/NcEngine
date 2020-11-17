#include "AddServerSelectableUIElement.h"

namespace
{
    const auto ELEMENT_SIZE = ImVec2{ 300, 100 };
}

namespace project::ui
{
    AddServerSelectableUIElement::AddServerSelectableUIElement(bool startOpen, ImVec2 dimensions, std::function<void(ServerSelectable)> callback)
        : nc::ui::IUIFixedElement(startOpen, nc::ui::UIPosition::Center, dimensions),
          AddServerCallback{callback}
    {
        m_nameBuffer[0] = '\0';
        m_ipBuffer[0] = '\0';
    }

    void AddServerSelectableUIElement::Draw()
    {
        if(!isOpen) return;

        nc::ui::IUIFixedElement::PositionElement();

        if(ImGui::Begin("Add Server", &(this->isOpen)))
        {
            ImGui::InputText("Nickname", m_nameBuffer, m_bufferSize);
            ImGui::InputText("IP", m_ipBuffer, m_bufferSize);

            if(ImGui::Button("Add"))
            {
                isOpen = false;
                AddServerCallback({std::string{m_nameBuffer}, std::string{m_ipBuffer}, false});
            }
        }
        ImGui::End();
    }
}