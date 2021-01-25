#ifdef NC_EDITOR_ENABLED
#pragma once

#include "ecs/Entity.h"
#include "ecs/EntityMap.h"

#include <optional>
#include <unordered_map>

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
            Editor(nc::graphics::Graphics * graphics);

            void Frame(float* dt, ecs::EntityMap& activeEntities);
            void ToggleGui() noexcept;
            void EnableGui() noexcept;
            void DisableGui() noexcept;
            bool IsGuiActive() const noexcept;

        private:
            nc::graphics::Graphics * m_graphics;
            bool m_isGuiActive;
            bool m_openState_UtilitiesPanel;
            bool m_openState_GraphicsResources;

            void DrawMenu();
            void DrawSceneGraphPanel(ecs::EntityMap& entities);
            void DrawUtilitiesPanel(float* dtMult);
            void DrawGraphicsResourcePanel();
            void DrawTimingControl(float* dtMult);
            void DrawEntityControl(nc::EntityHandle handle);
    };
}
#endif