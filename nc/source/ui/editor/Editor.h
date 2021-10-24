#ifdef NC_EDITOR_ENABLED
#pragma once

#include "Ecs.h"

namespace nc 
{
    namespace graphics 
    { 
        class Graphics;
    }
}

namespace nc::ui::editor
{
    class Editor
    {
        public:
            Editor(graphics::Graphics* graphics);
            void Frame(float* dt, registry_type* registry);

        private:
            nc::graphics::Graphics* m_graphics;
            bool m_openState_Editor;
            bool m_openState_UtilitiesPanel;

            void DrawMenu();
    };
}
#endif