#ifdef NC_EDITOR_ENABLED
#pragma once

#include "entity/Entity.h"
#include "ecs/EntityComponentSystem.h"

namespace nc 
{
    class Transform;
    namespace graphics { class Graphics; }
}

namespace nc::ui::editor
{
    class Editor
    {
        public:
            Editor(graphics::Graphics * graphics);
            void Frame(float* dt, ecs::EntityComponentSystem* ecs);

        private:
            nc::graphics::Graphics * m_graphics;
            bool m_openState_Editor;
            bool m_openState_UtilitiesPanel;

            void DrawMenu();
    };
}
#endif