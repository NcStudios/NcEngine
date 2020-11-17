#pragma once

#include "UIPosition.h"
#include "window/IOnResizeReceiver.h"
#include "imgui/imgui.h"

namespace nc::ui
{
    class UIElement
    {
        public:
            UIElement(bool startOpen);
            virtual ~UIElement() = default;

            void ToggleOpen();
            virtual void Draw() = 0;
            
            bool isOpen;
    };

    class UIFixedElement : public UIElement, public window::IOnResizeReceiver
    {
        public:
            UIFixedElement(bool startOpen, UIPosition position, ImVec2 dimensions);
            ~UIFixedElement();

            virtual void OnResize(nc::Vector2 dimensions);
        
        protected:
            void PositionElement();

        private:
            UIPosition m_position;
            /** @todo Once Vector2.ToImVec2() is implemented, this can be easily changed to ImVec2 */
            Vector2 m_screenDimensions;
            ImVec2 m_elementDimensions;
            ImVec2 m_topLeftPosition;

            void CalculateTopLeftPosition();
    };
} //end namespace nc::ui
