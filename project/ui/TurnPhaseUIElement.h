#pragma once

#include "UIElement.h"

namespace project::ui
{
    class TurnPhaseUIElement : public UIElement
    {
        public:
            TurnPhaseUIElement(bool startOpen);
            void Draw() override;
    };
}