#include "NewSceneDialog.h"

namespace
{
    constexpr nc::Vector2 WindowSize{400.0f, 300.0f};
}

namespace nc::editor
{
    NewSceneDialog::NewSceneDialog(UICallbacks::AddDialogCallbackType addDialogCallback)
        : DialogFixedCentered{WindowSize},
          m_addDialog{std::move(addDialogCallback)},
          m_onConfirm{}
    {
    }

    void NewSceneDialog::Open(DialogCallbacks::NewSceneOnConfirmCallbackType onConfirmCallback)
    {
        m_onConfirm = std::move(onConfirmCallback);
        m_addDialog(this);
        isOpen = true;
        m_buffer[0] = '\0';
    }

    void NewSceneDialog::Draw()
    {
        if(!isOpen) return;

        ImGui::SetNextWindowPos(GetPosition(), ImGuiCond_Always, ImVec2{0.5f, 0.5f});
        ImGui::SetNextWindowSize(GetSize());

        if(ImGui::Begin("Create New Scene"), &isOpen)
        {
            ImGui::SetWindowFocus();
            ImGui::Text("Scene Name: ");
            ImGui::SameLine();
            ImGui::InputText("", m_buffer, BufferSize);

            if(ImGui::Button("Create"))
            {
                if(m_onConfirm(std::string{m_buffer}))
                    isOpen = false;
            }

            ImGui::SameLine();

            if(ImGui::Button("Cancel"))
            {
                isOpen = false;
            }
        }
    }
}