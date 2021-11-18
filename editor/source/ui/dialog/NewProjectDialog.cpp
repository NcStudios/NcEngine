#include "NewProjectDialog.h"

#include <cstring>

namespace
{
    constexpr nc::Vector2 NewProjectDialogSize{350.0f, 120.0f};
}

namespace nc::editor
{
    NewProjectDialog::NewProjectDialog(UICallbacks::RegisterDialogCallbackType registerDialog)
        : DialogFixedCentered{"Create Project", NewProjectDialogSize},
          m_addDialog{std::move(registerDialog)},
          m_callback{}
    {
    }

    void NewProjectDialog::Open(DialogCallbacks::NewProjectOnConfirmCallbackType callback)
    {
        m_callback = std::move(callback);
        m_isOpen = true;
        m_addDialog(this);
        std::memset(m_nameBuffer, '\0', TextEntryBufferSize);
        std::memset(m_pathBuffer, '\0', TextEntryBufferSize);
    }

    void NewProjectDialog::Draw()
    {
        if(!m_isOpen) return;

        if(BeginWindow())
        {
            ImGui::Text("Project Name:");
            ImGui::SameLine();
            ImGui::InputText("##newprojectname", m_nameBuffer, TextEntryBufferSize);
            ImGui::Spacing();

            ImGui::Text("Directory:   ");
            ImGui::SameLine();
            ImGui::InputText("##newprojectpath", m_pathBuffer, TextEntryBufferSize);
            ImGui::Spacing();

            if(ImGui::Button("Ok"))
            {
                if(m_callback(std::string{m_nameBuffer}, std::filesystem::path{m_pathBuffer}))
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