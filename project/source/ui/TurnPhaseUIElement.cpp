#include "TurnPhaseUIElement.h"

namespace project::ui
{
    TurnPhaseUIElement::TurnPhaseUIElement(bool startOpen)
        : nc::ui::IUIElement(startOpen)
    {
    }

    void TurnPhaseUIElement::Draw()
    {
        if(!isOpen) return;

        if(ImGui::Begin("Turn Phases", &(this->isOpen)))
        {
            ImGui::Text("1) New Action Space");
            ImGui::Text("2) Replenish Spaces");
            ImGui::Text("3) Work");
            ImGui::Text("4) Return Home");
            ImGui::Text("5) Harvest");
        }
        ImGui::End();
    }
}