#pragma once

#include "ui/IUIElement.h"

namespace project::ui
{
    class SoundboardUIElement : public nc::ui::IUIElement
    {
        public:
            SoundboardUIElement(bool startOpen);
            void Draw() override;
        
        private:

    };
}