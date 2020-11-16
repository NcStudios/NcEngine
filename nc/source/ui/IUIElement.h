#pragma once

#include "imgui/imgui.h"
#include "window/IOnResizeReceiver.h"
#include "math/Vector2.h"

namespace nc::ui
{
    class IUIElement
    {
        public:
            IUIElement(bool startOpen)
                : isOpen(startOpen)
            {
            }

            virtual ~IUIElement() = default;

            void ToggleOpen()
            {
                isOpen = !isOpen;
            }

            virtual void Draw() = 0;
            
            bool isOpen;
    };

    class IUIElementCentered : public IUIElement, public window::IOnResizeReceiver
    {
        public:
            IUIElementCentered(bool startOpen, ImVec2 dimensions);
            ~IUIElementCentered();

            virtual void OnResize(nc::Vector2 dimensions);
        
        protected:
            Vector2 m_screenDimensions;
            ImVec2 m_elementDimensions;
            ImVec2 m_topLeftPosition;

            void PositionElement();

        private:
            void CalculateTopLeftPosition();
    };
} //end namespace nc::ui
