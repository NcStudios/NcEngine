#include "EditNameUIElement.h"
#include "NcConfig.h"

namespace project::ui
{
    EditNameUIElement::EditNameUIElement(bool startOpen, std::string initialName)
        : UIElement(startOpen)
    {
        initialName.copy(m_buffer, m_bufferSize);
    }

    void EditNameUIElement::Draw()
    {
        if(!isOpen) return;

        if(ImGui::Begin("Edit Player Name", &(this->isOpen)))
        {
            ImGui::InputText("Enter Name" , m_buffer, m_bufferSize);
            
            if(ImGui::Button("Save"))
            {
                nc::config::NcSetUserName(m_buffer);
            }
        }
        ImGui::End();
    }
}