#include "EditNameUIElement.h"
#include "Engine.h"

namespace project::ui
{
    EditNameUIElement::EditNameUIElement(bool startOpen)
        : UIElement(startOpen)
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
                nc::engine::Engine::SetUserName(m_buffer);
                isOpen = false;
            }
        }
        ImGui::End();
    }
}