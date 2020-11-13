#pragma once

#include "UIElement.h"
#include "ServerSelectable.h"

#include <vector>

namespace project::ui
{
    class AddServerSelectableUIElement : public UIElement
    {
        public:
            AddServerSelectableUIElement(bool startOpen, std::vector<ServerSelectable>& servers);
            void Draw() override;

        private:
            static const unsigned m_bufferSize = 16;
            char m_nameBuffer[m_bufferSize];
            char m_ipBuffer[m_bufferSize];
            std::vector<ServerSelectable>& m_servers;
    };
}