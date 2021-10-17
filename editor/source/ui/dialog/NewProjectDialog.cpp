#include "NewProjectDialog.h"

namespace
{
    constexpr nc::Vector2 NewProjectDialogSize{350.0f, 120.0f};
}

namespace nc::editor
{
    NewProjectDialog::NewProjectDialog(UICallbacks::AddDialogCallbackType addDialogCallback)
        : DialogFixedCentered{NewProjectDialogSize},
          m_addDialog{std::move(addDialogCallback)},
          m_callback{}
    {
    }

    void NewProjectDialog::Open(DialogCallbacks::NewProjectOnConfirmCallbackType callback)
    {
        m_callback = std::move(callback);
        isOpen = true;
        m_addDialog(this);
    }

    void NewProjectDialog::Draw()
    {
        if(!isOpen) return;

        ImGui::SetNextWindowPos(GetPosition(), ImGuiCond_Always, ImVec2{0.5f, 0.5f});
        ImGui::SetNextWindowSize(GetSize());

        if(ImGui::Begin("Create Project", &isOpen))
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
                    isOpen = false;
            }

            ImGui::SameLine();

            if(ImGui::Button("Cancel"))
            {
                isOpen = false;
            }
        }

        ImGui::End();
    }
}