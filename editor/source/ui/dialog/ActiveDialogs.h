#pragma once

#include "Dialog.h"

namespace nc::editor
{
    class ActiveDialogs
    {
        public:
            void Register(DialogBase* dialog);
            void Draw();

        private:
            std::vector<DialogBase*> m_dialogs;
            std::vector<DialogBase*> m_toAdd;
    };
}