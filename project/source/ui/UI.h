#pragma once
#include <memory>

#include "NcConfig.h"
#include "nc/source/ui/IUI.h"
#include "LogUIElement.h"
#include "TurnPhaseUIElement.h"
#include "EditNameUIElement.h"

namespace nc::graphics::d3dresource
{
    class Texture;
}

namespace project::log { class GameLog; }

namespace project::ui
{
    class UI : public nc::ui::IUI
    {
        public:
            UI(log::GameLog* gameLog);
            ~UI();
            void Draw() override;

        private:
            const ::nc::config::Config& m_config;
            LogUIElement m_logUIElement;
            TurnPhaseUIElement m_turnPhaseUIElement;
            EditNameUIElement m_editNameUIElement;
            std::unique_ptr<nc::graphics::d3dresource::Texture> m_texture;

            void DrawHUD();
            void DrawMenu();
            void DrawTurnHeader();
            void DrawResources();
            void DrawResource(nc::graphics::d3dresource::Texture* texture, unsigned count, const char* label);

    };
} //end namespace project::ui