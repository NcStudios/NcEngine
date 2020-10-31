#pragma once

#include "UIElement.h"
#include <string>

namespace project::ui
{
    class EditNameUIElement : public UIElement
    {
        public:
            EditNameUIElement(bool startOpen, std::string initialName);
            void Draw() override;
        
        private:
            static const unsigned m_bufferSize = 1024;
            char m_buffer[m_bufferSize];
    };
}