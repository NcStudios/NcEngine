#pragma once

#include "UIElement.h"

namespace project::ui
{
    class SoundboardUIElement : public UIElement
    {
        public:
            SoundboardUIElement(bool startOpen);
            void Draw() override;
        
        private:

    };
}