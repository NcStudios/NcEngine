#pragma once

#include "UIPosition.h"
#include "Window.h"

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
            UIFixedElement(bool startOpen, UIPosition position, Vector2 dimensions);
            ~UIFixedElement() noexcept;

            virtual void OnResize(Vector2 dimensions);
        
        protected:
            void PositionElement();

        private:
            UIPosition m_position;
            Vector2 m_screenDimensions;
            Vector2 m_elementDimensions;
            Vector2 m_topLeftPosition;

            void CalculateTopLeftPosition();
    };
} //end namespace nc::ui
