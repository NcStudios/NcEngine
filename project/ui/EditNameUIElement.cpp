#include "EditNameUIElement.h"

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

        if(ImGui::Begin("Player Name", &(this->isOpen)))
        {
            ImGui::InputText(m_buffer , m_buffer, m_bufferSize);
        }
        ImGui::End();
    }
}