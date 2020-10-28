#pragma once
#include <memory>

#include "LogUIElement.h"
#include "TurnPhaseUIElement.h"

namespace nc::graphics::d3dresource
{
    class Texture;
}

namespace project::ui
{
    class HUD
    {
        public:
            HUD();
            ~HUD();
            void Draw();

        private:
            LogUIElement m_logUIElement;
            TurnPhaseUIElement m_turnPhaseUIElement;
            std::unique_ptr<nc::graphics::d3dresource::Texture> m_texture;

            void DrawHUD();
            void DrawMenu();
            void DrawResources();
            void DrawResource(nc::graphics::d3dresource::Texture* texture, unsigned count, const char* label);

    };
} //end namespace project::ui