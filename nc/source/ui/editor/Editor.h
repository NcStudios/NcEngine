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
            Editor(graphics::Graphics2* graphics, const ecs::Systems& systems);
            #endif
            Editor(graphics::Graphics* graphics, const ecs::Systems& systems);

            void Frame(float* dt, ecs::EntityMap& activeEntities);

        private:
            #ifdef USE_VULKAN
            nc::graphics::Graphics2* m_graphics2;
            #endif
            nc::graphics::Graphics* m_graphics;
            ecs::Systems m_componentSystems;
            bool m_openState_Editor;
            bool m_openState_UtilitiesPanel;

            void DrawMenu();
    };
}
#endif