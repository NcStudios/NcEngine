#pragma once

#include "nc/source/ui/IUI.h"
#include "project/source/ui/EditNameUIElement.h"
#include "NcConfig.h"

#include <memory>

namespace project::ui
{
    class MainMenuUI : public nc::ui::IUI
    {
        public:
            MainMenuUI();
            ~MainMenuUI();
            void Draw() override;
            bool IsHovered() override;

        private:
            const nc::config::Config& m_config;
            bool m_isHovered;
            EditNameUIElement m_editNameElement;
    };
}