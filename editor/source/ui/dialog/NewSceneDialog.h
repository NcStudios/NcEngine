#pragma once

#include "Dialog.h"
#include "framework/Callbacks.h"

namespace nc::editor
{
    class NewSceneDialog : public DialogFixedCentered
    {
        using callback_type = std::function<bool(const std::string&)>;

        public:
            NewSceneDialog(UICallbacks::RegisterDialogCallbackType registerDialog);

            void Open(DialogCallbacks::NewSceneOnConfirmCallbackType onConfirmCallback);
            void Draw() override;

        private:
            UICallbacks::RegisterDialogCallbackType m_addDialog;
            DialogCallbacks::NewSceneOnConfirmCallbackType m_onConfirm;
            std::string m_buffer;
    };
}