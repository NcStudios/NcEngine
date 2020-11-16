#pragma once

#include "UIElementCentered.h"

#include <functional>
#include <string>

namespace project::ui
{
    class EditNameUIElement : public UIElementCentered
    {
        public:
            EditNameUIElement(bool startOpen, ImVec2 dimensions, std::function<void(std::string)> callback);

            void Draw() override;
        
        private:
            static const unsigned m_bufferSize = 64;
            char m_buffer[m_bufferSize];
            
            std::function<void(std::string)> EditNameCallback;
    };
}