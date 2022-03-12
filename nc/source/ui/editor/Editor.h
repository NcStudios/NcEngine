#ifdef NC_EDITOR_ENABLED
#pragma once

#include "ecs/Registry.h"

namespace nc::ui::editor
{
    class Editor
    {
        public:
            Editor();
            void Frame(float* dt, Registry* registry);

        private:
            bool m_openState_Editor;
            bool m_openState_UtilitiesPanel;

            void DrawMenu();
    };
}
#endif