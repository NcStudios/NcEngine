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
            Editor(graphics::Graphics* graphics, const ecs::Systems& systems);
            void Frame(float* dt, std::span<Entity*> activeEntities);

        private:
            nc::graphics::Graphics* m_graphics;
            ecs::Systems m_componentSystems;
            bool m_openState_Editor;
            bool m_openState_UtilitiesPanel;

            void DrawMenu();
    };
}
#endif