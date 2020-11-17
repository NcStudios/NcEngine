#include "EditNameUIElement.h"
#include "Engine.h"

namespace project::ui
{
    EditNameUIElement::EditNameUIElement(bool startOpen, std::function<void(std::string)> callback)
        : UIElement(startOpen),
          EditNameCallback{ callback }
    {
        m_buffer[0] = '\0';
    }

    void EditNameUIElement::Draw()
    {
        if(!isOpen) return;

        if(ImGui::Begin("Edit Player Name", &(this->isOpen)))
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