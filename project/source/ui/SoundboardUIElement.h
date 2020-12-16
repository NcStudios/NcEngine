#pragma once

#include "nc/source/ui/UIElement.h"

namespace project::ui
{
    class SoundboardUIElement : public nc::ui::UIElement
    {
        public:
            SoundboardUIElement(bool startOpen);
            void Draw() override;
        
        private:

    };
}