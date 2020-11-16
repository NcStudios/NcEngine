#pragma once

#include "nc/source/ui/IUI.h"
//#include "nc/source/window/IOnResizeReceiver.h"
#include "Engine.h"
//#include "math/Vector2.h"
#include "EditNameUIElement.h"
#include "AddServerSelectableUIElement.h"
#include "ServerSelectable.h"
#include "config/ProjectConfig.h"

#include <memory>
#include <vector>

namespace project::ui
{
    class MainMenuUI : public nc::ui::IUIFixed//, public nc::window::IOnResizeReceiver
    {
        public:
            MainMenuUI(config::ProjectConfig projectConfig);
            ~MainMenuUI();
            void Draw() override;
            bool IsHovered() override;
            //void OnResize(nc::Vector2 dimensions) override;

        private:
            void AddServer(ServerSelectable server);
            void EditName(std::string);

            config::ProjectConfig m_projectConfig;
            //nc::Vector2 m_screenDimensions;
            bool m_isHovered;
            std::vector<ServerSelectable> m_servers;
            EditNameUIElement m_editNameElement;
            AddServerSelectableUIElement m_addServerElement;
            static const unsigned m_ipBufferSize = 16u;
            char m_ipBuffer[m_ipBufferSize];
    };
}