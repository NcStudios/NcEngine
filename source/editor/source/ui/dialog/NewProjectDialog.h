#pragma once

#include "Dialog.h"
#include "framework/Callbacks.h"

namespace nc::editor
{
    class NewProjectDialog : public DialogFixedCentered
    {
        public:
            NewProjectDialog(UICallbacks::RegisterDialogCallbackType registerDialog);

            void Open(DialogCallbacks::NewProjectOnConfirmCallbackType callback);
            void Draw() override;

        private:
            UICallbacks::RegisterDialogCallbackType m_addDialog;
            DialogCallbacks::NewProjectOnConfirmCallbackType m_callback;
            std::string m_nameBuffer;
            std::string m_pathBuffer;
    };
}