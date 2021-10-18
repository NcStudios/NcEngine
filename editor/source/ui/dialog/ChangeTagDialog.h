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

            void RegisterregisterDialog(UICallbacks::RegisterDialogCallbackType registerDialog);

        private:
            registry_type* m_registry;
            UICallbacks::RegisterDialogCallbackType m_addDialog;
            Entity m_entity;
            char m_buffer[BufferSize];
    };
}