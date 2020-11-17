#include "EditNameUIElement.h"

namespace project::ui
{
    EditNameUIElement::EditNameUIElement(bool startOpen, ImVec2 dimensions, std::function<void(std::string)> callback)
        : nc::ui::IUIFixedElement(startOpen, nc::ui::UIPosition::Center, dimensions),
          EditNameCallback{ callback }
    {
        m_buffer[0] = '\0';
    }

    void EditNameUIElement::Draw()
    {
        if(!isOpen) return;

        nc::ui::IUIFixedElement::PositionElement();

        if(ImGui::Begin("Edit Player Name", &(this->isOpen), ImGuiWindowFlags_NoResize))
        {
            ImGui::InputText("" , m_buffer, m_bufferSize);
            
            if(ImGui::Button("Save"))
            {
                isOpen = false;
                EditNameCallback(std::string{m_buffer});
            }
        }
        ImGui::End();
    }
}