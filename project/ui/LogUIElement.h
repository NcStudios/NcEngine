#pragma once

#include "UIElement.h"

namespace project::ui
{
    class LogUIElement : public UIElement
    {
        public:
            LogUIElement(bool startOpen);
            void Draw() override;
    };
}