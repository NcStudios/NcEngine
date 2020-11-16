#pragma once

#include "UIElement.h"
#include "window/IOnResizeReceiver.h"
#include "math/Vector2.h"

namespace project::ui
{
    class UIElementCentered : public UIElement, public nc::window::IOnResizeReceiver
    {
        public:
            UIElementCentered(bool startOpen, ImVec2 dimensions);
            ~UIElementCentered();

            virtual void OnResize(nc::Vector2 dimensions);
        
        protected:
            nc::Vector2 m_screenDimensions;
            ImVec2 m_elementDimensions;
            ImVec2 m_topLeftPosition;

        private:
            void CalculateTopLeftPosition();
    };
}