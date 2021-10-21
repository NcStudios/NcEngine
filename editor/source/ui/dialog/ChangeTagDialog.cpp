#include "ChangeTagDialog.h"

namespace
{
    constexpr nc::Vector2 ChangeTagDialogSize{200.0f, 80.0f};
}

namespace nc::editor
{
    ChangeTagDialog::ChangeTagDialog(registry_type* registry)
        : DialogFixedCentered{"Change Entity Tag", ChangeTagDialogSize},
          m_registry{registry},
          m_addDialog{},
          m_entity{Entity::Null()}
    {
    }

    void ChangeTagDialog::Open(Entity entity)
    {
        m_isOpen = true;
        m_entity = entity;
        m_buffer[0] = '\0';
        m_addDialog(this);
    }

    void ChangeTagDialog::Draw()
    {
        if(!m_isOpen) return;
        
        if(BeginWindow())
        {
            ImGui::InputText("", m_buffer, BufferSize);
            if(ImGui::Button("Save"))
            {
                m_registry->Get<Tag>(m_entity)->Set(std::string{m_buffer});
                m_isOpen = false;
                m_entity = Entity::Null();
            }
        }

        EndWindow();
    }

    void ChangeTagDialog::RegisterDialog(UICallbacks::RegisterDialogCallbackType registerDialog)
    {
        m_addDialog = std::move(registerDialog);
    }
}