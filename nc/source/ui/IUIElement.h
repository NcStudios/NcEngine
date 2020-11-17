#pragma once

#include "UIPosition.h"
#include "window/IOnResizeReceiver.h"
#include "imgui/imgui.h"

namespace nc::ui
{
    class IUIElement
    {
        public:
            IUIElement(bool startOpen);
            virtual ~IUIElement() = default;

            void ToggleOpen();
            virtual void Draw() = 0;
            
            bool isOpen;
    };

    class IUIFixedElement : public IUIElement, public window::IOnResizeReceiver
    {
        public:
            IUIFixedElement(bool startOpen, UIPosition position, ImVec2 dimensions);
            ~IUIFixedElement();

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
