#ifdef NC_EDITOR_ENABLED
#pragma once

#include "Ecs.h"

namespace nc 
{
    class Transform;
    namespace graphics 
    { 
        class Graphics2;
    }
}

namespace nc::ui::editor
{
    class Editor
    {
        public:
            Editor(graphics::Graphics2* graphics);
            void Frame(float* dt, registry_type* registry);

        private:
            nc::graphics::Graphics2* m_graphics2;
            bool m_openState_Editor;
            bool m_openState_UtilitiesPanel;

            void DrawMenu();
    };
}
#endif