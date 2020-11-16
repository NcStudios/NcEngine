#pragma once

#include "ui/IUIElement.h"

namespace project::ui
{
    class TurnPhaseUIElement : public nc::ui::IUIElement
    {
        public:
            TurnPhaseUIElement(bool startOpen);
            void Draw() override;
    };
}