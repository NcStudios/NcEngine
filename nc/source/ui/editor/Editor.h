#ifdef NC_EDITOR_ENABLED
#pragma once

#include "entity/Entity.h"
#include "ecs/EntityComponentSystem.h"

namespace nc 
{
    class Transform;
    namespace graphics 
    { 
        class Graphics;
        #ifdef USE_VULKAN
        class Graphics2;
        #endif
    }
}

namespace nc::ui::editor
{
    class Editor
    {
        public:
            #ifdef USE_VULKAN
            Editor(graphics::Graphics2* graphics);
            #endif
            Editor(graphics::Graphics* graphics);

            void Frame(float* dt, ecs::registry_type* registry);

        private:
            #ifdef USE_VULKAN
            nc::graphics::Graphics2* m_graphics2;
            #endif
            nc::graphics::Graphics* m_graphics;
            bool m_openState_Editor;
            bool m_openState_UtilitiesPanel;

            void DrawMenu();
    };
}
#endif