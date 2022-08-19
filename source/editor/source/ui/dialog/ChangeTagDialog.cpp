#include "ChangeTagDialog.h"
#include "ui/ImGuiUtility.h"

namespace
{
    constexpr nc::Vector2 ChangeTagDialogSize{200.0f, 80.0f};
}

namespace nc::editor
{
    ChangeTagDialog::ChangeTagDialog(Registry* registry)
        : DialogFixedCentered{"Change Entity Tag", ChangeTagDialogSize},
          m_registry{registry},
          m_addDialog{},
          m_entity{Entity::Null()},
          m_buffer{}
    {
    }

    void ChangeTagDialog::Open(Entity entity)
    {
        m_isOpen = true;
        m_entity = entity;
        m_buffer.clear();
        m_buffer.reserve(64u);
        m_addDialog(this);
    }

    void ChangeTagDialog::Draw()
    {
        if(!m_isOpen) return;

        if(BeginWindow())
        {
            InputText("", &m_buffer);
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