#pragma once

#include "nc/source/ui/IUI.h"
#include "Engine.h"
#include "EditNameUIElement.h"
#include "AddServerSelectableUIElement.h"
#include "ServerSelectable.h"

#include <memory>
#include <vector>

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
            void AddServer(ServerSelectable server);
            
            const nc::config::Config& m_config;
            bool m_isHovered;
            std::vector<ServerSelectable> m_servers;
            EditNameUIElement m_editNameElement;
            AddServerSelectableUIElement m_addServerElement;
            static const unsigned m_ipBufferSize = 16u;
            char m_ipBuffer[m_ipBufferSize];
    };
}