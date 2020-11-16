#include "IUIElement.h"
#include "UI.h"
#include "Window.h"


namespace nc::ui
{
    IUIElementCentered::IUIElementCentered(bool startOpen, ImVec2 dimensions)
        : IUIElement(startOpen),
          m_screenDimensions { Window::GetDimensions() },
          m_elementDimensions{ dimensions }
    {
        CalculateTopLeftPosition();
        Window::RegisterOnResizeReceiver(this);
    }

    IUIElementCentered::~IUIElementCentered()
    {
        Window::UnregisterOnResizeReceiver(this);
    }

    void IUIElementCentered::OnResize(Vector2 dimensions)
    {
        m_screenDimensions = {dimensions.X(), dimensions.Y()};
        CalculateTopLeftPosition();
    }

    void IUIElementCentered::PositionElement()
    {
        ImGui::SetNextWindowPos(m_topLeftPosition);
        ImGui::SetNextWindowSize(m_elementDimensions);
    }

    void IUIElementCentered::CalculateTopLeftPosition()
    {
        m_topLeftPosition =
        {
            (m_screenDimensions.X() - m_elementDimensions.x) / 2.0f,
            (m_screenDimensions.Y() - m_elementDimensions.y) / 2.0f
        };
    }
}