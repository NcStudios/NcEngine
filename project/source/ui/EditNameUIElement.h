#pragma once

#include "UIElement.h"

#include <functional>
#include <string>

namespace project::ui
{
    class EditNameUIElement : public UIElement
    {
        public:
            EditNameUIElement(bool startOpen, std::function<void(std::string)> callback);
            void Draw() override;
        
        private:
            static const unsigned m_bufferSize = 64;
            char m_buffer[m_bufferSize];
            
            std::function<void(std::string)> EditNameCallback;
    };
}