#include "ChangeTagDialog.h"

namespace
{
    constexpr nc::Vector2 ChangeTagDialogSize{200.0f, 80.0f};
}

namespace nc::editor
{
    ChangeTagDialog::ChangeTagDialog(registry_type* registry)
        : DialogFixedCentered{ChangeTagDialogSize},
          m_registry{registry},
          m_addDialog{},
          m_entity{Entity::Null()}
    {
    }

    void ChangeTagDialog::Open(Entity entity)
    {
        isOpen = true;
        m_entity = entity;
        m_buffer[0] = '\0';
        m_addDialog(this);
    }

    void ChangeTagDialog::Draw()
    {
        if(!isOpen) return;

        ImGui::SetNextWindowPos(GetPosition(), ImGuiCond_Always, ImVec2{0.5f, 0.5f});
        ImGui::SetNextWindowSize(GetSize());
        if(ImGui::Begin("Change Entity Tag", &isOpen))
        {
            ImGui::InputText("", m_buffer, BufferSize);
            if(ImGui::Button("Save"))
            {
                m_registry->Get<Tag>(m_entity)->Set(std::string{m_buffer});
                isOpen = false;
                m_entity = Entity::Null();
            }
        } ImGui::End();
    }

    void ChangeTagDialog::RegisterAddDialogCallback(UICallbacks::AddDialogCallbackType addDialogCallback)
    {
        m_addDialog = std::move(addDialogCallback);
    }
}