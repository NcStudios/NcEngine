#pragma once

#include "IUIElement.h"

namespace nc::ui
{
    class IUI : public IUIElement
    {
        public:
            IUI()
                : IUIElement(true)
            {}

            virtual bool IsHovered() = 0;
    };

    class IUIPositioned : public IUIElementCentered
    {
        public:
            IUIPositioned(ImVec2 dimensions)
                : IUIElementCentered(true, dimensions)
            {
            }

            virtual bool IsHovered() = 0; 
    };
}