#pragma once

#include "ui/UIElement.h"

namespace project::ui
{
    class TurnPhaseUIElement : public nc::ui::UIElement
    {
        public:
            TurnPhaseUIElement(bool startOpen);
            void Draw() override;
    };
}