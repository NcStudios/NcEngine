#include "SoundboardUIElement.h"
#include "imgui/imgui.h"
#include "platform/win32/NcWinDef.h"
#include <MMSystem.h>

namespace
{
    auto buttonSize = ImVec2{ 128, 32 };
    auto daffyPath = TEXT("project/sounds/dumpTruckDriver.wav");
    auto admitItPath = TEXT("project/sounds/ohMaiGod.wav");
    auto flushMePath = TEXT("project/sounds/flushMeJay.wav");
    auto mudPiePath = TEXT("project/sounds/sloppyMudPie.wav");
    auto dotaPath = TEXT("project/sounds/dotaSong.wav");
    auto richmondPath = TEXT("project/sounds/fenton.wav");
}

namespace project::ui
{
    SoundboardUIElement::SoundboardUIElement(bool startOpen)
        : nc::ui::UIElement(startOpen)
    {
    }

    void SoundboardUIElement::Draw()
    {
        if(!isOpen) return;

        if(ImGui::Begin("Soundboard", &(this->isOpen)))
        {
            if(ImGui::Button("Tiny Dinky Daffy", buttonSize))
            {
                PlaySound(daffyPath, NULL, SND_ASYNC);
            }
            if(ImGui::Button("He admit it!", buttonSize))
            {
                PlaySound(admitItPath, NULL, SND_ASYNC);
            }
            if(ImGui::Button("Flush me J", buttonSize))
            {
                PlaySound(flushMePath, NULL, SND_ASYNC);
            }
            if(ImGui::Button("Sloppy mud pie!", buttonSize))
            {
                PlaySound(mudPiePath, NULL, SND_ASYNC);
            }
            if(ImGui::Button("Basshunter Dota", buttonSize))
            {
                PlaySound(dotaPath, NULL, SND_ASYNC);
            }
            if(ImGui::Button("Richmond Park", buttonSize))
            {
                PlaySound(richmondPath, NULL, SND_ASYNC);
            }
        }
        ImGui::End();
    }

} // end namespace project