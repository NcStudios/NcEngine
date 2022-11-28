#pragma once

#include "Dialog.h"
#include "framework/Callbacks.h"

#include "ncengine/ecs/Registry.h"

namespace nc::editor
{
    class ChangeTagDialog : public DialogFixedCentered
    {
        public:
            ChangeTagDialog(Registry* registry);

            void Open(Entity entity);
            void Draw() override;

            void RegisterDialog(UICallbacks::RegisterDialogCallbackType registerDialog);

        private:
            Registry* m_registry;
            UICallbacks::RegisterDialogCallbackType m_addDialog;
            Entity m_entity;
            std::string m_buffer;
    };
}