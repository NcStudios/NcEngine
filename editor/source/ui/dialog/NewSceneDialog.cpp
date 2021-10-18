#include "NewSceneDialog.h"

#include <cstring>

namespace
{
    constexpr nc::Vector2 WindowSize{400.0f, 300.0f};
}

namespace nc::editor
{
    NewSceneDialog::NewSceneDialog(UICallbacks::RegisterDialogCallbackType registerDialog)
        : DialogFixedCentered{"Create New Scene", WindowSize},
          m_addDialog{std::move(registerDialog)},
          m_onConfirm{}
    {
    }

    void NewSceneDialog::Open(DialogCallbacks::NewSceneOnConfirmCallbackType onConfirmCallback)
    {
        m_onConfirm = std::move(onConfirmCallback);
        m_addDialog(this);
        m_isOpen = true;
        std::memset(m_buffer, '\0', BufferSize);
    }

    void NewSceneDialog::Draw()
    {
        if(!m_isOpen) return;

        if(BeginWindow())
        {
            ImGui::SetWindowFocus();
            ImGui::Text("Scene Name: ");
            ImGui::SameLine();
            ImGui::InputText("", m_buffer, BufferSize);

            if(ImGui::Button("Create"))
            {
                if(m_onConfirm(std::string{m_buffer}))
                    m_isOpen = false;
            }

            ImGui::SameLine();

            if(ImGui::Button("Cancel"))
            {
                m_isOpen = false;
            }
        }

        EndWindow();
    }
}