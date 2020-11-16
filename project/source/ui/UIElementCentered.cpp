#include "UIElementCentered.h"
#include "UI.h"
#include "Window.h"


namespace project::ui
{
    UIElementCentered::UIElementCentered(bool startOpen, ImVec2 dimensions)
        : UIElement(startOpen),
          m_screenDimensions { nc::Window::GetDimensions() },
          m_elementDimensions{ dimensions }
    {
        CalculateTopLeftPosition();
        nc::Window::RegisterOnResizeReceiver(this);
    }

    UIElementCentered::~UIElementCentered()
    {
        nc::Window::UnregisterOnResizeReceiver(this);
    }

    void UIElementCentered::OnResize(nc::Vector2 dimensions)
    {
        m_screenDimensions = {dimensions.X(), dimensions.Y()};
        CalculateTopLeftPosition();
    }

    void UIElementCentered::CalculateTopLeftPosition()
    {
        m_topLeftPosition =
        {
            (m_screenDimensions.X() - m_elementDimensions.x) / 2.0f,
            (m_screenDimensions.Y() - m_elementDimensions.y) / 2.0f
        };
    }
}