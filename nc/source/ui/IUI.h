#pragma once

#include "UIElement.h"

namespace nc::ui
{
    /** @note For internal use only. Derived classes should 
     * inherit from UIFlexible or UIFixed. */
    class IUI
    {
        public:
            virtual ~IUI() = default;
            virtual void Draw() = 0;
            virtual bool IsHovered() = 0;
    };

    class UIFlexible : public IUI, public UIElement
    {
        public:
            UIFlexible()
                : UIElement(true)
            {
            }
    };

    class UIFixed : public IUI, public UIFixedElement
    {
        public:
            UIFixed(UIPosition position, ImVec2 dimensions)
                : UIFixedElement(true, position, dimensions)
            {
            }
    };
}