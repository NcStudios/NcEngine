#pragma once

#include "ui/IUIElement.h"
#include "math/Vector2.h"
#include "ServerSelectable.h"

#include <functional>

namespace project::ui
{
    class AddServerSelectableUIElement : public nc::ui::IUIElementCentered
    {
        public:
            AddServerSelectableUIElement(bool startOpen, ImVec2 dimensions, std::function<void(ServerSelectable)> callback);
            
            void Draw() override;

        private:
            static const unsigned m_bufferSize = 16;
            char m_nameBuffer[m_bufferSize];
            char m_ipBuffer[m_bufferSize];

            std::function<void(ServerSelectable)> AddServerCallback;
    };
}