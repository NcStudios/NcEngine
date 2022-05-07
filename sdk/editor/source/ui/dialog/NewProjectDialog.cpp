#include "NewProjectDialog.h"
#include "ui/ImGuiUtility.h"

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
          m_callback{},
          m_nameBuffer{},
          m_pathBuffer{}
    {
    }

    void NewProjectDialog::Open(DialogCallbacks::NewProjectOnConfirmCallbackType callback)
    {
        m_callback = std::move(callback);
        m_isOpen = true;
        m_addDialog(this);
        m_nameBuffer.clear();
        m_nameBuffer.reserve(64u);
        m_pathBuffer.clear();
        m_pathBuffer.reserve(64u);
    }

    void NewProjectDialog::Draw()
    {
        if(!m_isOpen) return;

        if(BeginWindow())
        {
            ImGui::Text("Project Name:");
            ImGui::SameLine();
            InputText("##newprojectname", &m_nameBuffer);
            ImGui::Spacing();

            ImGui::Text("Directory:   ");
            ImGui::SameLine();
            InputText("##newprojectpath", &m_pathBuffer);
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