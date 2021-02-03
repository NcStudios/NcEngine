#include "EditNameUIElement.h"
#include "imgui/imgui.h"

namespace
{
    const auto ELEMENT_SIZE = nc::Vector2{300, 100};
}

namespace project::ui
{
    EditNameUIElement::EditNameUIElement(bool startOpen, std::function<void(std::string)> callback)
        : nc::ui::UIFixedElement(startOpen, nc::ui::UIPosition::Center, ELEMENT_SIZE),
          EditNameCallback{ callback }
    {
        m_buffer[0] = '\0';
    }

    void EditNameUIElement::Draw()
    {
        if(!isOpen) return;

        nc::ui::UIFixedElement::PositionElement();

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