#include "AddServerSelectableUIElement.h"

namespace project::ui
{
    AddServerSelectableUIElement::AddServerSelectableUIElement(bool startOpen, std::vector<ServerSelectable>& servers)
        : UIElement(startOpen),
          m_servers{servers}
    {
        m_nameBuffer[0] = '\0';
        m_ipBuffer[0] = '\0';
    }

    void AddServerSelectableUIElement::Draw()
    {
        if(!isOpen) return;

        if(ImGui::Begin("Add Server", &(this->isOpen)))
        {
            ImGui::InputText("Nickname", m_nameBuffer, m_bufferSize);

            ImGui::InputText("IP", m_ipBuffer, m_bufferSize);

            if(ImGui::Button("Add"))
            {
                isOpen = false;
                m_servers.push_back({std::string{m_nameBuffer}, std::string{m_ipBuffer}, false});
            }
        }
        ImGui::End();
    }
}