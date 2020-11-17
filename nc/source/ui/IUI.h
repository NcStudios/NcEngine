#pragma once

#include "IUIElement.h"

namespace nc::ui
{
    /** @note For internal use only. Derived classes should 
     * inherit from  IUI or IUIFixed. */
    class IUIBase
    {
        public:
            virtual ~IUIBase() = default;
            virtual void Draw() = 0;
            virtual bool IsHovered() = 0;
    };

    class IUI : public IUIBase, public IUIElement
    {
        public:
            IUI()
                : IUIElement(true)
            {
            }
    };

    class IUIFixed : public IUIBase, public IUIFixedElement
    {
        public:
            IUIFixed(UIPosition position, ImVec2 dimensions)
                : IUIFixedElement(true, position, dimensions)
            {
            }
    };
}