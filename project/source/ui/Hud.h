#pragma once

#include "nc/source/ui/IUI.h"
#include "config/ProjectConfig.h"
#include "LogUIElement.h"
#include "TurnPhaseUIElement.h"
#include "SoundboardUIElement.h"

#include <memory>

namespace nc::graphics::d3dresource
{
    class Texture;
}

namespace project::log { class GameLog; }

namespace project::ui
{
    class Hud : public nc::ui::IUI
    {
        public:
            Hud(log::GameLog* gameLog, config::ProjectConfig projectConfig);
            ~Hud();
            void Draw() override;
            bool IsHovered() override;

        private:
            config::ProjectConfig m_projectConfig;
            LogUIElement m_logUIElement;
            TurnPhaseUIElement m_turnPhaseUIElement;
            SoundboardUIElement m_soundboardUIElement;
            std::unique_ptr<nc::graphics::d3dresource::Texture> m_texture;
            bool m_isHovered;

            void DrawHUD();
            void DrawMenu();
            void DrawTurnHeader();
            void DrawResources();
            void DrawResource(nc::graphics::d3dresource::Texture* texture, unsigned count, const char* label);
    };
} //end namespace project::ui