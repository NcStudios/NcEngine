#pragma once

#include "UIElement.h"
#include <string>

namespace project::ui
{
    class EditNameUIElement : public UIElement
    {
        public:
            EditNameUIElement(bool startOpen);
            void Draw() override;
        
        private:
            static const unsigned m_bufferSize = 64;
            char m_buffer[m_bufferSize];
    };
}