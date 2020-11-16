#include "EditNameUIElement.h"
#include "Engine.h"

namespace project::ui
{
    EditNameUIElement::EditNameUIElement(bool startOpen, ImVec2 dimensions, std::function<void(std::string)> callback)
        : UIElementCentered(startOpen, dimensions),
          EditNameCallback{ callback }
    {
        m_buffer[0] = '\0';
    }

    void EditNameUIElement::Draw()
    {
        if(!isOpen) return;

        ImGui::SetNextWindowPos(m_topLeftPosition);
        ImGui::SetNextWindowSize(m_elementDimensions);

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