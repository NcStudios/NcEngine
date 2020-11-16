#pragma once

#include "imgui/imgui.h"

namespace project::ui
{
    class UIElement
    {
        public:
            UIElement(bool startOpen)
                : isOpen(startOpen)
            {
            }

            virtual ~UIElement() = default;

            void ToggleOpen()
            {
                isOpen = !isOpen;
            }

            virtual void Draw() = 0;
            
            bool isOpen;
    };
} //end namespace ui
