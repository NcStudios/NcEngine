#pragma once

#include "Dialog.h"
#include "framework/Callbacks.h"

namespace nc::editor
{
    class NewProjectDialog : public DialogFixedCentered
    {
        inline static constexpr size_t TextEntryBufferSize = 256;

        public:
            NewProjectDialog(UICallbacks::RegisterDialogCallbackType registerDialog);

            void Open(DialogCallbacks::NewProjectOnConfirmCallbackType callback);
            void Draw() override;

        private:
            UICallbacks::RegisterDialogCallbackType m_addDialog;
            DialogCallbacks::NewProjectOnConfirmCallbackType m_callback;
            char m_nameBuffer[TextEntryBufferSize];
            char m_pathBuffer[TextEntryBufferSize];
    };
}