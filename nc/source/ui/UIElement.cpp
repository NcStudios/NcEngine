#include "UIElement.h"
#include "Window.h"

namespace nc::ui
{
    UIElement::UIElement(bool startOpen)
        : isOpen(startOpen)
    {
    }

    void UIElement::ToggleOpen()
    {
        isOpen = !isOpen;
    }

    UIFixedElement::UIFixedElement(bool startOpen, UIPosition position, ImVec2 dimensions)
        : UIElement(startOpen),
          m_position{ position },
          m_screenDimensions { Window::GetDimensions() },
          m_elementDimensions{ dimensions }
    {
        CalculateTopLeftPosition();
        Window::RegisterOnResizeReceiver(this);
    }

    UIFixedElement::~UIFixedElement()
    {
        Window::UnregisterOnResizeReceiver(this);
    }

    void UIFixedElement::OnResize(Vector2 dimensions)
    {
        m_screenDimensions = {dimensions.X(), dimensions.Y()};
        CalculateTopLeftPosition();
    }

    void UIFixedElement::PositionElement()
    {
        ImGui::SetNextWindowPos(m_topLeftPosition);
        ImGui::SetNextWindowSize(m_elementDimensions);
    }

    void UIFixedElement::CalculateTopLeftPosition()
    {
        m_topLeftPosition = utils::GetTopLeftCoords( m_position, {m_screenDimensions.X(), m_screenDimensions.Y()}, m_elementDimensions);
    }
}