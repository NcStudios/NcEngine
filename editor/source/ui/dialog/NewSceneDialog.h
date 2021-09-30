#pragma once

#include "Dialog.h"
#include "framework/Callbacks.h"

namespace nc::editor
{
    class NewSceneDialog : public DialogFixedCentered
    {
        using callback_type = std::function<bool(const std::string&)>;
        static constexpr size_t BufferSize = 64u;

        public:
            NewSceneDialog(UICallbacks::AddDialogCallbackType addDialogCallback);

            void Open(DialogCallbacks::NewSceneOnConfirmCallbackType onConfirmCallback);
            void Draw() override;

        private:
            UICallbacks::AddDialogCallbackType m_addDialog;
            DialogCallbacks::NewSceneOnConfirmCallbackType m_onConfirm;
            char m_buffer[BufferSize];
    };
}