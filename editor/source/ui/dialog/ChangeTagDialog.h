#pragma once

#include "Dialog.h"
#include "framework/Callbacks.h"

namespace nc::editor
{
    class ChangeTagDialog : public DialogFixedCentered
    {
        static constexpr size_t BufferSize = 64;

        public:
            ChangeTagDialog(registry_type* registry);

            void Open(Entity entity);
            void Draw() override;

            void RegisterAddDialogCallback(UICallbacks::AddDialogCallbackType addDialogCallback);

        private:
            registry_type* m_registry;
            UICallbacks::AddDialogCallbackType m_addDialog;
            Entity m_entity;
            char m_buffer[BufferSize];
    };
}