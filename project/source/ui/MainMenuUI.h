#pragma once

#include "nc/source/ui/IUI.h"
#include "EditNameUIElement.h"
#include "AddServerSelectableUIElement.h"
#include "ServerSelectable.h"
#include "config/ProjectConfig.h"

#include <memory>
#include <vector>

namespace project::ui
{
    class MainMenuUI : public nc::ui::UIFixed
    {
        public:
            MainMenuUI(config::ProjectConfig projectConfig);
            ~MainMenuUI();
            void Draw() override;
            bool IsHovered() override;

        private:
            void AddServer(ServerSelectable server);
            void EditName(std::string);

            config::ProjectConfig m_projectConfig;
            bool m_isHovered;
            std::vector<ServerSelectable> m_servers;
            EditNameUIElement m_editNameElement;
            AddServerSelectableUIElement m_addServerElement;
            static const unsigned m_ipBufferSize = 16u;
            char m_ipBuffer[m_ipBufferSize];
    };
}